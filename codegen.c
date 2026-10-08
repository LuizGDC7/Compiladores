#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "symbol_table.h"

static int label_counter = 0;
static int string_counter = 0;
static SymbolTableStack *cg_stack = NULL;
static FILE *cg_out_data = NULL;

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

static void gen_expression(ASTNode *node, FILE *out) {
    if (!node) return;

    switch (node->type) {
        case NODE_INT_CONST:
            fprintf(out, "    li $t0, %d\n", node->int_val);
            break;

        case NODE_CAR_CONST:
            fprintf(out, "    li $t0, %d # '%c'\n", (int)node->char_val, node->char_val);
            break;

        case NODE_VAR: {
            Symbol *sym = lookup_symbol(cg_stack, node->string_val);
            if (sym) {
                fprintf(out, "    lw $t0, var_%d # %s\n", sym->id, sym->name);
            }
            break;
        }

        case NODE_ASSIGN: {
            gen_expression(node->children[1], out);
            ASTNode *var_node = node->children[0];
            Symbol *sym = lookup_symbol(cg_stack, var_node->string_val);
            if (sym) {
                fprintf(out, "    sw $t0, var_%d # %s\n", sym->id, sym->name);
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

static void gen_statement(ASTNode *node, FILE *out) {
    if (!node) return;

    switch (node->type) {
        case NODE_BLOCK:
            push_scope(cg_stack);
            if (node->num_children > 0 && node->children[0]) {
                ASTNode *decl_list = node->children[0];
                for (int i = 0; i < decl_list->num_children; i++) {
                    ASTNode *decl = decl_list->children[i];
                    if (decl->type == NODE_VAR_DECL) {
                        Symbol *sym = insert_symbol(cg_stack, decl->string_val, decl->data_type, decl->line);
                        if (cg_out_data) {
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
                    fprintf(out, "    li $v0, 12\n");
                    fprintf(out, "    syscall\n");
                    fprintf(out, "    sw $v0, var_%d\n", sym->id);
                } else {
                    fprintf(out, "    li $v0, 5\n");
                    fprintf(out, "    syscall\n");
                    fprintf(out, "    sw $v0, var_%d\n", sym->id);
                }
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

void generate_code(ASTNode *root, FILE *out) {
    if (!root || !out) return;

    cg_out_data = out;
    fprintf(out, ".data\n");
    fprintf(out, "newline_str: .asciiz \"\\n\"\n");

    cg_stack = init_symbol_table_stack();

    char *text_buffer = NULL;
    size_t text_size = 0;
    FILE *mem_stream = open_memstream(&text_buffer, &text_size);

    fprintf(mem_stream, "\n.text\n");
    fprintf(mem_stream, ".globl main\n");
    fprintf(mem_stream, "main:\n");

    if (root->type == NODE_PROGRAM && root->num_children > 0) {
        gen_statement(root->children[0], mem_stream);
    } else {
        gen_statement(root, mem_stream);
    }

    fprintf(mem_stream, "    li $v0, 10\n");
    fprintf(mem_stream, "    syscall\n");
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
    cg_out_data = NULL;
}
