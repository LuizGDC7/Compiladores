#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "symbol_table.h"

/*
 * [G-V2] ORGANIZAÇÃO DA MEMÓRIA
 *
 * No G-V1 toda variável era um rótulo em .data. Isso não funciona com recursão,
 * pois cada ativação de uma função precisa de cópias próprias das variáveis.
 * Agora:
 *   - globais continuam em .data, com rótulos g_<nome>;
 *   - parâmetros e locais ficam na pilha, acessados por deslocamento de $fp.
 *
 * Registro de ativação (a pilha cresce para endereços menores):
 *
 *      | argumento 1    |  $fp + 8 + 4*(n-1)
 *      | ...            |
 *      | argumento n    |  $fp + 8
 *      | $ra salvo      |  $fp + 4
 *      | $fp anterior   |  $fp + 0   <- $fp
 *      | locais do bloco|  $fp - 4, $fp - 8, ...
 *      | temporários    |  <- $sp
 *
 * Convenção de chamada:
 *   - o chamador empilha os argumentos da esquerda para a direita, executa jal
 *     e remove os argumentos depois do retorno;
 *   - o chamado salva $ra e $fp, e devolve o resultado em $v0.
 *
 * Vetores: toda variável vetor é um "slot" de 4 bytes contendo o ENDEREÇO do
 * primeiro elemento. Isso implementa diretamente duas regras do enunciado:
 *   - passar vetor como argumento = passar o conteúdo do slot (referência);
 *   - "Y = X" entre vetores = copiar o endereço de X para o slot de Y.
 * Cada elemento ocupa uma palavra (4 bytes), inclusive em vetores de car.
 */

static int label_counter = 0;
static int string_counter = 0;
static SymbolTableStack *cg_stack = NULL;

/* [G-V2] Próximo deslocamento livre abaixo de $fp no registro de ativação. */
static int cg_frame_offset = 0;
/* [G-V2] Rótulo para onde o comando retorne salta na função atual. */
static char cg_return_label[300];

typedef struct StringLiteral {
    int id;
    char *text;
    struct StringLiteral *next;
} StringLiteral;

static StringLiteral *str_list_head = NULL;

static int add_string_literal(const char *text) {
    StringLiteral *curr = str_list_head;
    while (curr) {
        if (strcmp(curr->text, text) == 0) {
            return curr->id;
        }
        curr = curr->next;
    }
    StringLiteral *node = (StringLiteral*) malloc(sizeof(StringLiteral));
    node->id = ++string_counter;
    node->text = strdup(text ? text : "");
    node->next = str_list_head;
    str_list_head = node;
    return node->id;
}

static void gen_expression(ASTNode *node, FILE *out);
static void gen_statement(ASTNode *node, FILE *out);

/* [G-V2] Carrega em reg o conteúdo do slot do símbolo (global ou na pilha). */
static void emit_load_slot(Symbol *sym, const char *reg, FILE *out) {
    if (sym->is_global) {
        fprintf(out, "    lw %s, g_%s\n", reg, sym->name);
    } else {
        fprintf(out, "    lw %s, %d($fp) # %s\n", reg, sym->stack_offset, sym->name);
    }
}

/* [G-V2] Grava $t0 no slot do símbolo (global ou na pilha). */
static void emit_store_slot(Symbol *sym, FILE *out) {
    if (sym->is_global) {
        fprintf(out, "    sw $t0, g_%s\n", sym->name);
    } else {
        fprintf(out, "    sw $t0, %d($fp) # %s\n", sym->stack_offset, sym->name);
    }
}

static void emit_push_t0(FILE *out) {
    fprintf(out, "    subu $sp, $sp, 4\n");
    fprintf(out, "    sw $t0, 0($sp)\n");
}

/*
 * [G-V2] Calcula em $t0 o endereço de v[indice]: base + indice * 4.
 * A base é lida depois do índice porque o cálculo do índice pode conter chamadas
 * de função, que alteram $t1.
 */
static void gen_element_address(ASTNode *node, FILE *out) {
    Symbol *sym = lookup_symbol(cg_stack, node->string_val);
    gen_expression(node->children[0], out);
    fprintf(out, "    sll $t0, $t0, 2\n");
    emit_load_slot(sym, "$t1", out);
    fprintf(out, "    addu $t0, $t1, $t0\n");
}

static void gen_expression(ASTNode *node, FILE *out) {
    if (!node) return;

    switch (node->type) {
        case NODE_INT_CONST:
            fprintf(out, "    li $t0, %d\n", node->int_val);
            break;

        case NODE_CAR_CONST:
            /* [G-V2] O caractere saiu do comentário: '\n' quebraria a linha do assembly. */
            fprintf(out, "    li $t0, %d\n", (int)node->char_val);
            break;

        /* [G-V2] Para um vetor sem índice, o slot já contém o endereço do vetor. */
        case NODE_VAR: {
            Symbol *sym = lookup_symbol(cg_stack, node->string_val);
            if (sym) {
                emit_load_slot(sym, "$t0", out);
            }
            break;
        }

        /* [G-V2] Leitura de um elemento de vetor. */
        case NODE_ARRAY_ACCESS:
            gen_element_address(node, out);
            fprintf(out, "    lw $t0, 0($t0)\n");
            break;

        /*
         * [G-V2] Chamada: avalia e empilha os argumentos, salta para a função,
         * remove os argumentos e deixa o resultado em $t0, como toda expressão.
         */
        case NODE_CALL:
            for (int i = 0; i < node->num_children; i++) {
                gen_expression(node->children[i], out);
                emit_push_t0(out);
            }
            fprintf(out, "    jal f_%s\n", node->string_val);
            if (node->num_children > 0) {
                fprintf(out, "    addu $sp, $sp, %d\n", 4 * node->num_children);
            }
            fprintf(out, "    move $t0, $v0\n");
            break;

        case NODE_ASSIGN: {
            ASTNode *target = node->children[0];
            if (target->type == NODE_ARRAY_ACCESS) {
                /* [G-V2] Guarda o endereço na pilha enquanto avalia o lado direito. */
                gen_element_address(target, out);
                emit_push_t0(out);
                gen_expression(node->children[1], out);
                fprintf(out, "    lw $t1, 0($sp)\n");
                fprintf(out, "    addu $sp, $sp, 4\n");
                fprintf(out, "    sw $t0, 0($t1)\n");
            } else {
                gen_expression(node->children[1], out);
                Symbol *sym = lookup_symbol(cg_stack, target->string_val);
                if (sym) {
                    emit_store_slot(sym, out);
                }
            }
            break;
        }

        case NODE_BIN_OP: {
            gen_expression(node->children[0], out);
            fprintf(out, "    subu $sp, $sp, 4\n");
            fprintf(out, "    sw $t0, 0($sp)\n");

            gen_expression(node->children[1], out);

            fprintf(out, "    lw $t1, 0($sp)\n");
            fprintf(out, "    addu $sp, $sp, 4\n");

            OperatorType op = (OperatorType) node->int_val;
            switch (op) {
                case OP_ADD:
                    fprintf(out, "    add $t0, $t1, $t0\n");
                    break;
                case OP_SUB:
                    fprintf(out, "    sub $t0, $t1, $t0\n");
                    break;
                case OP_MUL:
                    fprintf(out, "    mul $t0, $t1, $t0\n");
                    break;
                case OP_DIV:
                    fprintf(out, "    div $t1, $t0\n");
                    fprintf(out, "    mflo $t0\n");
                    break;
                case OP_IGUAL:
                    fprintf(out, "    seq $t0, $t1, $t0\n");
                    break;
                case OP_DIFERENTE:
                    fprintf(out, "    sne $t0, $t1, $t0\n");
                    break;
                case OP_LESS:
                    fprintf(out, "    slt $t0, $t1, $t0\n");
                    break;
                case OP_GREATER:
                    fprintf(out, "    sgt $t0, $t1, $t0\n");
                    break;
                case OP_GE:
                    fprintf(out, "    sge $t0, $t1, $t0\n");
                    break;
                case OP_LE:
                    fprintf(out, "    sle $t0, $t1, $t0\n");
                    break;
                case OP_OU:
                    fprintf(out, "    sne $t1, $t1, $zero\n");
                    fprintf(out, "    sne $t0, $t0, $zero\n");
                    fprintf(out, "    or $t0, $t1, $t0\n");
                    break;
                case OP_E:
                    fprintf(out, "    sne $t1, $t1, $zero\n");
                    fprintf(out, "    sne $t0, $t0, $zero\n");
                    fprintf(out, "    and $t0, $t1, $t0\n");
                    break;
                default:
                    break;
            }
            break;
        }

        case NODE_UN_OP: {
            gen_expression(node->children[0], out);
            OperatorType op = (OperatorType) node->int_val;
            if (op == OP_NEG) {
                fprintf(out, "    neg $t0, $t0\n");
            } else if (op == OP_NOT) {
                fprintf(out, "    seq $t0, $t0, $zero\n");
            }
            break;
        }

        default:
            break;
    }
}

/*
 * [G-V2] Bloco: reserva as locais na pilha ao entrar e libera ao sair, de modo
 * que o tempo de vida da variável seja exatamente a duração do bloco.
 * Blocos são comandos, logo nunca há temporários de expressão na pilha aqui.
 */
static void gen_block(ASTNode *node, FILE *out) {
    ASTNode *decl_list = node->children[0];
    int saved_offset = cg_frame_offset;
    int block_size = 0;

    push_scope(cg_stack);

    /* 1ª passada: calcula o deslocamento de cada variável. */
    for (int i = 0; i < decl_list->num_children; i++) {
        ASTNode *decl = decl_list->children[i];
        Symbol *sym = insert_variable(cg_stack, decl->string_val, decl->data_type, i + 1, decl->line);
        cg_frame_offset -= 4;
        block_size += 4;
        sym->stack_offset = cg_frame_offset;
        if (decl->is_vector) {
            /* Os elementos ficam logo abaixo do slot que guarda o endereço. */
            cg_frame_offset -= 4 * decl->array_size;
            block_size += 4 * decl->array_size;
        }
    }

    if (block_size > 0) {
        fprintf(out, "    subu $sp, $sp, %d\n", block_size);
    }

    /* 2ª passada: cada slot de vetor recebe o endereço do seu primeiro elemento. */
    for (int i = 0; i < decl_list->num_children; i++) {
        ASTNode *decl = decl_list->children[i];
        if (decl->is_vector) {
            Symbol *sym = lookup_symbol_current_scope(cg_stack, decl->string_val);
            fprintf(out, "    addiu $t0, $fp, %d\n", sym->stack_offset - 4 * decl->array_size);
            emit_store_slot(sym, out);
        }
    }

    gen_statement(node->children[1], out);

    if (block_size > 0) {
        fprintf(out, "    addu $sp, $sp, %d\n", block_size);
    }

    cg_frame_offset = saved_offset;
    pop_scope(cg_stack);
}

static void gen_statement(ASTNode *node, FILE *out) {
    if (!node) return;

    switch (node->type) {
        case NODE_BLOCK:
            gen_block(node, out);
            break;

        case NODE_CMD_LIST:
            for (int i = 0; i < node->num_children; i++) {
                gen_statement(node->children[i], out);
            }
            break;

        case NODE_CMD_EXPR:
            if (node->num_children > 0) {
                gen_expression(node->children[0], out);
            }
            break;

        /* [G-V2] Põe o valor em $v0 e salta para o epílogo da função atual. */
        case NODE_CMD_RETURN:
            gen_expression(node->children[0], out);
            fprintf(out, "    move $v0, $t0\n");
            fprintf(out, "    j %s\n", cg_return_label);
            break;

        /* [G-V2] O destino do leia pode ser uma variável ou um elemento de vetor. */
        case NODE_CMD_LEIA: {
            ASTNode *target = node->children[0];
            fprintf(out, "    li $v0, %d\n", node->data_type == TYPE_CAR ? 12 : 5);
            fprintf(out, "    syscall\n");
            if (target->type == NODE_ARRAY_ACCESS) {
                /* $v0 é salvo porque o índice pode conter uma chamada de função. */
                fprintf(out, "    move $t0, $v0\n");
                emit_push_t0(out);
                gen_element_address(target, out);
                fprintf(out, "    lw $t1, 0($sp)\n");
                fprintf(out, "    addu $sp, $sp, 4\n");
                fprintf(out, "    sw $t1, 0($t0)\n");
            } else {
                Symbol *sym = lookup_symbol(cg_stack, target->string_val);
                fprintf(out, "    move $t0, $v0\n");
                emit_store_slot(sym, out);
            }
            break;
        }

        case NODE_CMD_ESCREVA_EXPR:
            if (node->num_children > 0) {
                gen_expression(node->children[0], out);
                fprintf(out, "    move $a0, $t0\n");
                if (node->children[0]->data_type == TYPE_CAR) {
                    fprintf(out, "    li $v0, 11\n");
                } else {
                    fprintf(out, "    li $v0, 1\n");
                }
                fprintf(out, "    syscall\n");
            }
            break;

        case NODE_CMD_ESCREVA_STRING: {
            int str_id = add_string_literal(node->string_val);
            fprintf(out, "    la $a0, str_%d\n", str_id);
            fprintf(out, "    li $v0, 4\n");
            fprintf(out, "    syscall\n");
            break;
        }

        case NODE_CMD_NOVALINHA:
            fprintf(out, "    la $a0, newline_str\n");
            fprintf(out, "    li $v0, 4\n");
            fprintf(out, "    syscall\n");
            break;

        case NODE_CMD_IF: {
            int lbl_else = ++label_counter;
            int lbl_end = ++label_counter;

            gen_expression(node->children[0], out);
            fprintf(out, "    beqz $t0, L_else_%d\n", lbl_else);

            gen_statement(node->children[1], out);
            fprintf(out, "    j L_end_%d\n", lbl_end);

            fprintf(out, "L_else_%d:\n", lbl_else);
            if (node->num_children > 2 && node->children[2]) {
                gen_statement(node->children[2], out);
            }
            fprintf(out, "L_end_%d:\n", lbl_end);
            break;
        }

        case NODE_CMD_WHILE: {
            int lbl_start = ++label_counter;
            int lbl_end = ++label_counter;

            fprintf(out, "L_start_%d:\n", lbl_start);
            gen_expression(node->children[0], out);
            fprintf(out, "    beqz $t0, L_end_%d\n", lbl_end);

            gen_statement(node->children[1], out);
            fprintf(out, "    j L_start_%d\n", lbl_start);

            fprintf(out, "L_end_%d:\n", lbl_end);
            break;
        }

        default:
            break;
    }
}

/*
 * [G-V2] Variáveis globais em .data. Um vetor global tem dois rótulos:
 * g_v (slot com o endereço) e gd_v (área dos elementos). Os prefixos diferentes
 * impedem colisão: com "g_v_dados", uma global chamada v_dados teria o mesmo rótulo.
 */
static void gen_globals(ASTNode *globals, FILE *out) {
    for (int i = 0; i < globals->num_children; i++) {
        ASTNode *decl = globals->children[i];
        Symbol *sym = insert_variable(cg_stack, decl->string_val, decl->data_type, i + 1, decl->line);
        fprintf(out, ".align 2\n");
        if (decl->is_vector) {
            fprintf(out, "g_%s: .word gd_%s\n", sym->name, sym->name);
            fprintf(out, "gd_%s: .space %d\n", sym->name, 4 * decl->array_size);
        } else {
            fprintf(out, "g_%s: .word 0\n", sym->name);
        }
    }
}

/*
 * [G-V2] Gera uma função: prólogo, corpo e epílogo. O prefixo f_ evita conflito
 * entre o nome da função e instruções MIPS ou o rótulo main.
 */
static void gen_function(ASTNode *func, FILE *out) {
    ASTNode *params = func->children[0];
    int n = params->num_children;

    /* O prefixo fim_ não colide com f_: "f_x_fim" seria o rótulo de uma função x_fim. */
    snprintf(cg_return_label, sizeof(cg_return_label), "fim_%s", func->string_val);

    fprintf(out, "\nf_%s:\n", func->string_val);
    fprintf(out, "    subu $sp, $sp, 8\n");
    fprintf(out, "    sw $ra, 4($sp)\n");
    fprintf(out, "    sw $fp, 0($sp)\n");
    fprintf(out, "    move $fp, $sp\n");

    /* Os parâmetros têm deslocamento positivo: foram empilhados pelo chamador. */
    push_scope(cg_stack);
    for (int i = 0; i < n; i++) {
        ASTNode *param = params->children[i];
        Symbol *sym = insert_parameter(cg_stack, param->string_val, param->data_type, i + 1, param->line);
        sym->stack_offset = 8 + 4 * (n - 1 - i);
    }

    cg_frame_offset = 0;
    gen_statement(func->children[1], out);
    pop_scope(cg_stack);

    /* Função que termina sem retorne devolve 0. */
    fprintf(out, "    move $v0, $zero\n");
    fprintf(out, "%s:\n", cg_return_label);
    fprintf(out, "    move $sp, $fp\n");
    fprintf(out, "    lw $fp, 0($sp)\n");
    fprintf(out, "    lw $ra, 4($sp)\n");
    fprintf(out, "    addu $sp, $sp, 8\n");
    fprintf(out, "    jr $ra\n");
}

void generate_code(ASTNode *root, FILE *out) {
    if (!root || !out) return;

    fprintf(out, ".data\n");

    cg_stack = init_symbol_table_stack();
    push_scope(cg_stack); /* [G-V2] escopo global */
    gen_globals(root->children[0], out);

    fprintf(out, "newline_str: .asciiz \"\\n\"\n");

    /* O texto é gerado em memória porque as strings são descobertas durante o percurso. */
    char *text_buffer = NULL;
    size_t text_size = 0;
    FILE *mem_stream = open_memstream(&text_buffer, &text_size);

    fprintf(mem_stream, "\n.text\n");
    fprintf(mem_stream, ".globl main\n");
    fprintf(mem_stream, "main:\n");
    fprintf(mem_stream, "    move $fp, $sp\n"); /* [G-V2] base das locais do principal */

    /* [G-V2] retorne dentro de principal encerra o programa. */
    snprintf(cg_return_label, sizeof(cg_return_label), "principal_fim");
    cg_frame_offset = 0;
    gen_statement(root->children[2], mem_stream);

    fprintf(mem_stream, "principal_fim:\n");
    fprintf(mem_stream, "    li $v0, 10\n");
    fprintf(mem_stream, "    syscall\n");

    /* [G-V2] As funções ficam depois do principal; a execução começa em main. */
    ASTNode *functions = root->children[1];
    for (int i = 0; i < functions->num_children; i++) {
        gen_function(functions->children[i], mem_stream);
    }
    fclose(mem_stream);

    StringLiteral *s = str_list_head;
    while (s) {
        fprintf(out, "str_%d: .asciiz %s\n", s->id, s->text);
        s = s->next;
    }

    if (text_buffer) {
        fputs(text_buffer, out);
        free(text_buffer);
    }

    free_symbol_table_stack(cg_stack);
    cg_stack = NULL;
}
