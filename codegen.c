/*
 * =============================================================================
 * codegen.c — Implementação da Geração de Código Assembly MIPS
 * =============================================================================
 *
 * RESPONSABILIDADE:
 *   Percorrer a AST mais uma vez para emitir código Assembly.
 *   
 * ARQUITETURA ALVO: MIPS
 *   - $t0, $t1... : registradores temporários para cálculos de expressões.
 *   - $a0 : registrador de argumento para syscalls.
 *   - $v0 : registrador para resultado de syscalls / funções.
 *   - Pilha ($sp) : usada para cálculos complexos (empilhar resultados 
 *                   intermediários em operações binárias).
 *
 * ESTRATÉGIA:
 *   1. Strings literais (ex: escreva "ola") são coletadas separadamente e
 *      emitidas na seção `.data` no final do arquivo.
 *   2. Variáveis globais são emitidas na seção `.data` como words (4 bytes).
 *      (O G-V1 simplifica alocando tudo na .data com nomes "var_ID").
 *   3. O código executável vai para a seção `.text`.
 *   4. Como precisamos gerar as variáveis `.data` à medida que andamos na
 *      árvore, usamos buffers de memória (open_memstream) para separar o
 *      código (text) dos dados (.data), juntando tudo no final.
 * =============================================================================
 */

#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "symbol_table.h"

static int label_counter = 0;   /* Contador para gerar labels únicos (if/while) */
static int string_counter = 0;  /* Contador para gerar labels únicos de strings */
static SymbolTableStack *cg_stack = NULL; /* Pilha de escopos para a geração */
static FILE *cg_out_data = NULL; /* Arquivo onde imprimiremos as declarações .data */

/*
 * Estrutura para armazenar Strings Literais
 * Todas as strings encontradas num `escreva "texto";` vão para uma lista.
 */
typedef struct StringLiteral {
    int id;
    char *text;
    struct StringLiteral *next;
} StringLiteral;

static StringLiteral *str_list_head = NULL;

/* Adiciona a string na lista e retorna seu ID (para formar "str_ID") */
static int add_string_literal(const char *text) {
    StringLiteral *curr = str_list_head;
    while (curr) {
        if (strcmp(curr->text, text) == 0) {
            return curr->id; /* Reutiliza a string se já existir igual */
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

/*
 * gen_expression — Gera Assembly para calcular uma expressão
 * -----------------------------------------------------------------------
 * O resultado de todas as expressões é colocado sempre no registrador $t0.
 */
static void gen_expression(ASTNode *node, FILE *out) {
    if (!node) return;

    switch (node->type) {
        case NODE_INT_CONST:
            /* Carrega um valor imediato em $t0 */
            fprintf(out, "    li $t0, %d\n", node->int_val);
            break;

        case NODE_CAR_CONST:
            /* Carrega o valor ASCII do caractere em $t0 */
            fprintf(out, "    li $t0, %d # '%c'\n", (int)node->char_val, node->char_val);
            break;

        case NODE_VAR: {
            /* Lê valor da memória (ram) para $t0 */
            Symbol *sym = lookup_symbol(cg_stack, node->string_val);
            if (sym) {
                fprintf(out, "    lw $t0, var_%d # %s\n", sym->id, sym->name);
            }
            break;
        }

        case NODE_ASSIGN: {
            /* Executa expressão do lado direito (resultado em $t0) */
            gen_expression(node->children[1], out);
            ASTNode *var_node = node->children[0];
            /* Salva $t0 na variável correspondente na memória */
            Symbol *sym = lookup_symbol(cg_stack, var_node->string_val);
            if (sym) {
                fprintf(out, "    sw $t0, var_%d # %s\n", sym->id, sym->name);
            }
            break;
        }

        case NODE_BIN_OP: {
            /* Avalia filho esquerdo e EMPILHA o resultado ($t0 -> pilha) */
            gen_expression(node->children[0], out);
            fprintf(out, "    subu $sp, $sp, 4\n"); /* Aumenta a pilha */
            fprintf(out, "    sw $t0, 0($sp)\n");   /* Salva o valor nela */

            /* Avalia filho direito (novo resultado vai para $t0) */
            gen_expression(node->children[1], out);

            /* DESEMPILHA o esquerdo para $t1, e diminui a pilha */
            fprintf(out, "    lw $t1, 0($sp)\n");
            fprintf(out, "    addu $sp, $sp, 4\n");

            /* Realiza a operação entre $t1 (esq) e $t0 (dir) e salva em $t0 */
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
                    fprintf(out, "    div $t1, $t0\n"); /* div em MIPS salva em HI/LO */
                    fprintf(out, "    mflo $t0\n");      /* pega o quociente em LO */
                    break;
                case OP_IGUAL:
                    fprintf(out, "    seq $t0, $t1, $t0\n"); /* seq = set if equal (1 se igual, 0 senao) */
                    break;
                case OP_DIFERENTE:
                    fprintf(out, "    sne $t0, $t1, $t0\n"); /* sne = set not equal */
                    break;
                case OP_LESS:
                    fprintf(out, "    slt $t0, $t1, $t0\n"); /* slt = set less than */
                    break;
                case OP_GREATER:
                    fprintf(out, "    sgt $t0, $t1, $t0\n"); /* sgt = set greater than */
                    break;
                case OP_GE:
                    fprintf(out, "    sge $t0, $t1, $t0\n"); /* sge = set greater equal */
                    break;
                case OP_LE:
                    fprintf(out, "    sle $t0, $t1, $t0\n"); /* sle = set less equal */
                    break;
                case OP_OU:
                    fprintf(out, "    sne $t1, $t1, $zero\n"); /* Normaliza para 1 ou 0 */
                    fprintf(out, "    sne $t0, $t0, $zero\n");
                    fprintf(out, "    or $t0, $t1, $t0\n");
                    break;
                case OP_E:
                    fprintf(out, "    sne $t1, $t1, $zero\n"); /* Normaliza para 1 ou 0 */
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
                fprintf(out, "    seq $t0, $t0, $zero\n"); /* Se for zero vira 1, senao 0 */
            }
            break;
        }

        default:
            break;
    }
}

/*
 * gen_statement — Gera Assembly para comandos estruturais
 * -----------------------------------------------------------------------
 */
static void gen_statement(ASTNode *node, FILE *out) {
    if (!node) return;

    switch (node->type) {
        case NODE_BLOCK:
            push_scope(cg_stack);
            
            /* Para cada variável do bloco, alocamos ela na seção .data */
            if (node->num_children > 0 && node->children[0]) {
                ASTNode *decl_list = node->children[0];
                for (int i = 0; i < decl_list->num_children; i++) {
                    ASTNode *decl = decl_list->children[i];
                    if (decl->type == NODE_VAR_DECL) {
                        /* Inserir no escopo garante que acharemos a ID da variável (var_ID) */
                        Symbol *sym = insert_symbol(cg_stack, decl->string_val, decl->data_type, decl->line);
                        if (cg_out_data) {
                            /* .word 0 inicializa variáveis com 0 */
                            fprintf(cg_out_data, "var_%d: .word 0 # %s (line %d)\n", sym->id, sym->name, sym->line);
                        }
                    }
                }
            }
            if (node->num_children > 1 && node->children[1]) {
                gen_statement(node->children[1], out);
            }
            pop_scope(cg_stack);
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

        case NODE_CMD_LEIA: {
            Symbol *sym = lookup_symbol(cg_stack, node->string_val);
            if (sym) {
                if (sym->type == TYPE_CAR) {
                    /* syscall 12 = read character */
                    fprintf(out, "    li $v0, 12\n");
                    fprintf(out, "    syscall\n");
                    fprintf(out, "    sw $v0, var_%d\n", sym->id);
                } else {
                    /* syscall 5 = read integer */
                    fprintf(out, "    li $v0, 5\n");
                    fprintf(out, "    syscall\n");
                    fprintf(out, "    sw $v0, var_%d\n", sym->id);
                }
            }
            break;
        }

        case NODE_CMD_ESCREVA_EXPR:
            if (node->num_children > 0) {
                gen_expression(node->children[0], out); /* Valor calculado vai estar em $t0 */
                fprintf(out, "    move $a0, $t0\n");    /* Prepara valor em $a0 para syscall */
                if (node->children[0]->data_type == TYPE_CAR) {
                    /* syscall 11 = print character */
                    fprintf(out, "    li $v0, 11\n");
                } else {
                    /* syscall 1 = print integer */
                    fprintf(out, "    li $v0, 1\n");
                }
                fprintf(out, "    syscall\n");
            }
            break;

        case NODE_CMD_ESCREVA_STRING: {
            int str_id = add_string_literal(node->string_val);
            /* syscall 4 = print string (terminada em \0) */
            fprintf(out, "    la $a0, str_%d\n", str_id);
            fprintf(out, "    li $v0, 4\n");
            fprintf(out, "    syscall\n");
            break;
        }

        case NODE_CMD_NOVALINHA:
            /* A string de nova linha já fica declarada na raiz da geracao de codigo */
            fprintf(out, "    la $a0, newline_str\n");
            fprintf(out, "    li $v0, 4\n");
            fprintf(out, "    syscall\n");
            break;

        case NODE_CMD_IF: {
            int lbl_else = ++label_counter;
            int lbl_end = ++label_counter;

            gen_expression(node->children[0], out); /* Avalia a condição em $t0 */
            /* beqz (branch if equal to zero): pula pro "else" se $t0 == 0 */
            fprintf(out, "    beqz $t0, L_else_%d\n", lbl_else);

            gen_statement(node->children[1], out); /* Executa "entao" */
            fprintf(out, "    j L_end_%d\n", lbl_end); /* Pula pro fim */

            fprintf(out, "L_else_%d:\n", lbl_else);
            if (node->num_children > 2 && node->children[2]) {
                gen_statement(node->children[2], out); /* Executa "senao" (opcional) */
            }
            fprintf(out, "L_end_%d:\n", lbl_end);
            break;
        }

        case NODE_CMD_WHILE: {
            int lbl_start = ++label_counter;
            int lbl_end = ++label_counter;

            fprintf(out, "L_start_%d:\n", lbl_start);
            gen_expression(node->children[0], out); /* Avalia a condicao */
            
            /* Se a condição for falsa (0), pula pro fim do laço */
            fprintf(out, "    beqz $t0, L_end_%d\n", lbl_end);

            gen_statement(node->children[1], out); /* Executa o corpo do laço */
            fprintf(out, "    j L_start_%d\n", lbl_start); /* Volta pro começo */

            fprintf(out, "L_end_%d:\n", lbl_end);
            break;
        }

        default:
            break;
    }
}

/*
 * generate_code — Ponto de entrada público
 * -----------------------------------------------------------------------
 */
void generate_code(ASTNode *root, FILE *out) {
    if (!root || !out) return;

    cg_out_data = out; /* Arquivo principal aponta para out */
    
    /* Prepara a Seção de Dados (Data Segment) do MIPS */
    fprintf(out, ".data\n");
    fprintf(out, "newline_str: .asciiz \"\\n\"\n");

    /* Pilha temporária para emular os escopos */
    cg_stack = init_symbol_table_stack();

    /* 
     * Truque C-POSIX (open_memstream): Cria um buffer em memória "mem_stream"
     * simulando um arquivo, para gravar todo o código (Text Segment).
     * Isso permite escrever as .data em "out" durante o traversal, mas guardar
     * o código pra imprimir depois, no mesmo arquivo final.
     */
    char *text_buffer = NULL;
    size_t text_size = 0;
    FILE *mem_stream = open_memstream(&text_buffer, &text_size);

    fprintf(mem_stream, "\n.text\n");
    fprintf(mem_stream, ".globl main\n");
    fprintf(mem_stream, "main:\n");

    /* Percorre a arvore preenchendo o mem_stream (text) e o cg_out_data (.data) */
    if (root->type == NODE_PROGRAM && root->num_children > 0) {
        gen_statement(root->children[0], mem_stream);
    } else {
        gen_statement(root, mem_stream);
    }

    /* Encerra programa no syscall MIPS: v0=10 (exit) */
    fprintf(mem_stream, "    li $v0, 10\n");
    fprintf(mem_stream, "    syscall\n");
    fclose(mem_stream);

    /* Grava as strings literais acumuladas no "out" (.data) */
    StringLiteral *s = str_list_head;
    while (s) {
        fprintf(out, "str_%d: .asciiz %s\n", s->id, s->text);
        s = s->next;
    }

    /* Grava o buffer do text_buffer no arquivo de saída final */
    if (text_buffer) {
        fputs(text_buffer, out);
        free(text_buffer);
    }

    free_symbol_table_stack(cg_stack);
    cg_stack = NULL;
    cg_out_data = NULL;
}
