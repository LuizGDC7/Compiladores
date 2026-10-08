#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"
#include "symbol_table.h"

static SymbolTableStack *sym_stack = NULL;

static void semantic_error(const char *msg, int line) {
    printf("ERRO: %s %d\n", msg, line);
    exit(1);
}

static DataType check_expression(ASTNode *node);
static void check_statement(ASTNode *node);
static void check_block(ASTNode *node);
static void process_declarations(ASTNode *decl_list);

static DataType check_expression(ASTNode *node) {
    if (!node) return TYPE_NONE;

    switch (node->type) {
        case NODE_INT_CONST:
            node->data_type = TYPE_INT;
            return TYPE_INT;

        case NODE_CAR_CONST:
            node->data_type = TYPE_CAR;
            return TYPE_CAR;

        case NODE_VAR: {
            Symbol *sym = lookup_symbol(sym_stack, node->string_val);
            if (!sym) {
                semantic_error("VARIAVEL NAO DECLARADA", node->line);
            }
            node->data_type = sym->type;
            return sym->type;
        }

        case NODE_ASSIGN: {
            ASTNode *var_node = node->children[0];
            ASTNode *expr_node = node->children[1];
            Symbol *sym = lookup_symbol(sym_stack, var_node->string_val);
            if (!sym) {
                semantic_error("VARIAVEL NAO DECLARADA", var_node->line);
            }
            var_node->data_type = sym->type;

            DataType rhs_type = check_expression(expr_node);
            if (sym->type != rhs_type) {
                semantic_error("TIPOS INCOMPATIVEIS NA ATRIBUICAO", node->line);
            }
            node->data_type = sym->type;
            return node->data_type;
        }

        case NODE_BIN_OP: {
            ASTNode *left = node->children[0];
            ASTNode *right = node->children[1];
            DataType t1 = check_expression(left);
            DataType t2 = check_expression(right);
            OperatorType op = (OperatorType) node->int_val;

            if (op == OP_ADD || op == OP_SUB || op == OP_MUL || op == OP_DIV) {
                if (t1 != TYPE_INT || t2 != TYPE_INT) {
                    semantic_error("OPERANDOS ARITMETICOS DEVEM SER INT", node->line);
                }
                node->data_type = TYPE_INT;
            } else if (op == OP_OU || op == OP_E) {
                if (t1 != TYPE_INT || t2 != TYPE_INT) {
                    semantic_error("OPERANDOS LOGICOS DEVEM SER INT", node->line);
                }
                node->data_type = TYPE_INT;
            } else {
                if (t1 != t2) {
                    semantic_error("TIPOS INCOMPATIVEIS NA COMPARACAO", node->line);
                }
                node->data_type = TYPE_INT;
            }
            return node->data_type;
        }

        case NODE_UN_OP: {
            ASTNode *expr = node->children[0];
            DataType t = check_expression(expr);
            OperatorType op = (OperatorType) node->int_val;

            if (op == OP_NEG || op == OP_NOT) {
                if (t != TYPE_INT) {
                    semantic_error("OPERANDO UNARIO DEVE SER INT", node->line);
                }
            }
            node->data_type = TYPE_INT;
            return TYPE_INT;
        }

        default:
            return TYPE_NONE;
    }
}

static void process_declarations(ASTNode *decl_list) {
    if (!decl_list) return;
    for (int i = 0; i < decl_list->num_children; i++) {
        ASTNode *decl = decl_list->children[i];
        if (decl->type == NODE_VAR_DECL) {
            const char *var_name = decl->string_val;
            DataType type = decl->data_type;
            int line = decl->line;

            if (lookup_symbol_current_scope(sym_stack, var_name) != NULL) {
                semantic_error("VARIAVEL JA DECLARADA", line);
            }
            insert_symbol(sym_stack, var_name, type, line);
        }
    }
}

static void check_statement(ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case NODE_BLOCK:
            check_block(node);
            break;

        case NODE_CMD_EXPR:
            if (node->num_children > 0) {
                check_expression(node->children[0]);
            }
            break;

        case NODE_CMD_LEIA: {
            Symbol *sym = lookup_symbol(sym_stack, node->string_val);
            if (!sym) {
                semantic_error("VARIAVEL NAO DECLARADA", node->line);
            }
            node->data_type = sym->type;
            break;
        }

        case NODE_CMD_ESCREVA_EXPR:
            if (node->num_children > 0) {
                check_expression(node->children[0]);
            }
            break;

        case NODE_CMD_ESCREVA_STRING:
        case NODE_CMD_NOVALINHA:
            break;

        case NODE_CMD_IF: {
            DataType cond_type = check_expression(node->children[0]);
            if (cond_type != TYPE_INT) {
                semantic_error("CONDICAO DO SE DEVE SER INT", node->children[0]->line);
            }
            check_statement(node->children[1]);
            if (node->num_children > 2 && node->children[2]) {
                check_statement(node->children[2]);
            }
            break;
        }

        case NODE_CMD_WHILE: {
            DataType cond_type = check_expression(node->children[0]);
            if (cond_type != TYPE_INT) {
                semantic_error("CONDICAO DO ENQUANTO DEVE SER INT", node->children[0]->line);
            }
            check_statement(node->children[1]);
            break;
        }

        case NODE_CMD_LIST:
            for (int i = 0; i < node->num_children; i++) {
                check_statement(node->children[i]);
            }
            break;

        default:
            break;
    }
}

static void check_block(ASTNode *node) {
    if (!node || node->type != NODE_BLOCK) return;

    push_scope(sym_stack);

    if (node->num_children > 0 && node->children[0]) {
        process_declarations(node->children[0]);
    }

    if (node->num_children > 1 && node->children[1]) {
        check_statement(node->children[1]);
    }

    pop_scope(sym_stack);
}

int analyze_semantics(ASTNode *root) {
    if (!root) return 0;
    sym_stack = init_symbol_table_stack();

    if (root->type == NODE_PROGRAM && root->num_children > 0) {
        check_block(root->children[0]);
    } else if (root->type == NODE_BLOCK) {
        check_block(root);
    }

    free_symbol_table_stack(sym_stack);
    sym_stack = NULL;

    return 0;
}
