/*
 * =============================================================================
 * semantic.c — Implementação da Análise Semântica
 * =============================================================================
 *
 * RESPONSABILIDADE:
 *   Implementar a checagem de regras semânticas na AST gerada pelo parser.
 *   Garante que o programa faz sentido do ponto de vista de tipos e regras
 *   da linguagem antes de prosseguirmos para a geração de código.
 *
 * PRINCIPAIS VERIFICAÇÕES:
 *   1. Declaração de Variáveis:
 *      - Variáveis são declaradas antes do uso?
 *      - Não há redeclaração no mesmo escopo?
 *   2. Checagem de Tipos:
 *      - O lado esquerdo e direito de uma atribuição têm o mesmo tipo?
 *      - Operações aritméticas (+, -, *, /) são feitas apenas com inteiros?
 *      - Condições de if/while resultam em tipo inteiro (booleano)?
 *
 * FLUXO DE EXECUÇÃO:
 *   A função `analyze_semantics` inicia a tabela de símbolos (stack) vazia.
 *   O percurso na árvore (Tree Traversal) é feito descendo recursivamente
 *   (check_block -> check_statement -> check_expression).
 *   Ao entrar em um bloco, um novo escopo é criado (push_scope).
 *   Ao sair do bloco, o escopo é destruído (pop_scope).
 * =============================================================================
 */

#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"
#include "symbol_table.h"

/* Pilha global de tabela de símbolos usada durante a análise semântica. */
static SymbolTableStack *sym_stack = NULL;

/*
 * semantic_error — Reporta um erro semântico e aborta
 * -----------------------------------------------------------------------
 * Exibe a mensagem de erro formatada com a linha onde ocorreu e encerra
 * o compilador.
 */
static void semantic_error(const char *msg, int line) {
    printf("ERRO: %s %d\n", msg, line);
    exit(1);
}

/* Protótipos de funções locais */
static DataType check_expression(ASTNode *node);
static void check_statement(ASTNode *node);
static void check_block(ASTNode *node);
static void process_declarations(ASTNode *decl_list);

/*
 * check_expression — Verifica a semântica de uma expressão
 * -----------------------------------------------------------------------
 * Percorre nós de expressão recursivamente.
 * Infere o tipo resultante da expressão (DataType) e verifica se os
 * operandos são compatíveis com a operação solicitada.
 *
 * Retorna o tipo resultante (TYPE_INT ou TYPE_CAR).
 */
static DataType check_expression(ASTNode *node) {
    if (!node) return TYPE_NONE;

    switch (node->type) {
        case NODE_INT_CONST:
            node->data_type = TYPE_INT; /* Um literal inteiro é do tipo INT */
            return TYPE_INT;

        case NODE_CAR_CONST:
            node->data_type = TYPE_CAR; /* Um literal caractere é do tipo CAR */
            return TYPE_CAR;

        case NODE_VAR: {
            /* Uso de variável: precisamos checar se ela foi declarada */
            Symbol *sym = lookup_symbol(sym_stack, node->string_val);
            if (!sym) {
                semantic_error("VARIAVEL NAO DECLARADA", node->line);
            }
            node->data_type = sym->type;
            return sym->type;
        }

        case NODE_ASSIGN: {
            /* Atribuição (ex: x = 5) */
            ASTNode *var_node = node->children[0];
            ASTNode *expr_node = node->children[1];
            
            /* Verifica se a variável do lado esquerdo foi declarada */
            Symbol *sym = lookup_symbol(sym_stack, var_node->string_val);
            if (!sym) {
                semantic_error("VARIAVEL NAO DECLARADA", var_node->line);
            }
            var_node->data_type = sym->type;

            /* Verifica o tipo da expressão do lado direito */
            DataType rhs_type = check_expression(expr_node);
            
            /* Os tipos dos dois lados devem ser idênticos */
            if (sym->type != rhs_type) {
                semantic_error("TIPOS INCOMPATIVEIS NA ATRIBUICAO", node->line);
            }
            node->data_type = sym->type;
            return node->data_type;
        }

        case NODE_BIN_OP: {
            /* Operações Binárias (+, -, ==, etc) */
            ASTNode *left = node->children[0];
            ASTNode *right = node->children[1];
            DataType t1 = check_expression(left);
            DataType t2 = check_expression(right);
            OperatorType op = (OperatorType) node->int_val;

            if (op == OP_ADD || op == OP_SUB || op == OP_MUL || op == OP_DIV) {
                /* Aritmética exige inteiros */
                if (t1 != TYPE_INT || t2 != TYPE_INT) {
                    semantic_error("OPERANDOS ARITMETICOS DEVEM SER INT", node->line);
                }
                node->data_type = TYPE_INT;
            } else if (op == OP_OU || op == OP_E) {
                /* Operações lógicas exigem inteiros */
                if (t1 != TYPE_INT || t2 != TYPE_INT) {
                    semantic_error("OPERANDOS LOGICOS DEVEM SER INT", node->line);
                }
                node->data_type = TYPE_INT;
            } else {
                /* Comparações (==, !=, >, <) exigem que ambos os lados tenham o mesmo tipo */
                if (t1 != t2) {
                    semantic_error("TIPOS INCOMPATIVEIS NA COMPARACAO", node->line);
                }
                node->data_type = TYPE_INT; /* O resultado de uma comparação é lógico (INT) */
            }
            return node->data_type;
        }

        case NODE_UN_OP: {
            /* Operações unárias (!, -) */
            ASTNode *expr = node->children[0];
            DataType t = check_expression(expr);
            OperatorType op = (OperatorType) node->int_val;

            if (op == OP_NEG || op == OP_NOT) {
                /* Ambas exigem operandos inteiros no momento */
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

/*
 * process_declarations — Insere variáveis na tabela de símbolos
 * -----------------------------------------------------------------------
 * Recebe uma lista de nós de declaração (NODE_VAR_DECL) e insere cada um
 * na tabela de símbolos do escopo atual.
 * Verifica redeclaração (variável já existente no MESMO escopo).
 */
static void process_declarations(ASTNode *decl_list) {
    if (!decl_list) return;
    for (int i = 0; i < decl_list->num_children; i++) {
        ASTNode *decl = decl_list->children[i];
        if (decl->type == NODE_VAR_DECL) {
            const char *var_name = decl->string_val;
            DataType type = decl->data_type;
            int line = decl->line;

            /* Impede duas variáveis com o mesmo nome no mesmo bloco { } */
            if (lookup_symbol_current_scope(sym_stack, var_name) != NULL) {
                semantic_error("VARIAVEL JA DECLARADA", line);
            }
            insert_symbol(sym_stack, var_name, type, line);
        }
    }
}

/*
 * check_statement — Verifica a semântica de um comando
 * -----------------------------------------------------------------------
 * Identifica o tipo de comando (If, While, Leia, Escreva, etc.) e dispara
 * as validações correspondentes.
 */
static void check_statement(ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case NODE_BLOCK:
            check_block(node); /* Bloco interno aninhado */
            break;

        case NODE_CMD_EXPR:
            if (node->num_children > 0) {
                check_expression(node->children[0]);
            }
            break;

        case NODE_CMD_LEIA: {
            /* Comando 'leia': variável precisa estar declarada */
            Symbol *sym = lookup_symbol(sym_stack, node->string_val);
            if (!sym) {
                semantic_error("VARIAVEL NAO DECLARADA", node->line);
            }
            node->data_type = sym->type;
            break;
        }

        case NODE_CMD_ESCREVA_EXPR:
            if (node->num_children > 0) {
                check_expression(node->children[0]); /* A expressão precisa ser válida */
            }
            break;

        case NODE_CMD_ESCREVA_STRING:
        case NODE_CMD_NOVALINHA:
            /* Nada a verificar semanticamente para imprimir strings fixas ou quebras de linha */
            break;

        case NODE_CMD_IF: {
            /* Condição do IF tem que resolver para inteiro (booleano) */
            DataType cond_type = check_expression(node->children[0]);
            if (cond_type != TYPE_INT) {
                semantic_error("CONDICAO DO SE DEVE SER INT", node->children[0]->line);
            }
            check_statement(node->children[1]); /* Bloco ENTÃO */
            if (node->num_children > 2 && node->children[2]) {
                check_statement(node->children[2]); /* Bloco SENÃO (opcional) */
            }
            break;
        }

        case NODE_CMD_WHILE: {
            /* Condição do WHILE tem que resolver para inteiro */
            DataType cond_type = check_expression(node->children[0]);
            if (cond_type != TYPE_INT) {
                semantic_error("CONDICAO DO ENQUANTO DEVE SER INT", node->children[0]->line);
            }
            check_statement(node->children[1]); /* Corpo do laço */
            break;
        }

        case NODE_CMD_LIST:
            /* Lista de comandos executados em sequência */
            for (int i = 0; i < node->num_children; i++) {
                check_statement(node->children[i]);
            }
            break;

        default:
            break;
    }
}

/*
 * check_block — Verifica um bloco de código { ... }
 * -----------------------------------------------------------------------
 * Ao entrar num bloco, cria um novo escopo (push).
 * Processa eventuais declarações locais, em seguida verifica os comandos.
 * Ao final do bloco, destrói o escopo (pop).
 */
static void check_block(ASTNode *node) {
    if (!node || node->type != NODE_BLOCK) return;

    /* Entra em novo escopo */
    push_scope(sym_stack);

    /* Index 0 é a área de declaração de variáveis (VarSection) */
    if (node->num_children > 0 && node->children[0]) {
        process_declarations(node->children[0]);
    }

    /* Index 1 é a lista de comandos do bloco (Comando) */
    if (node->num_children > 1 && node->children[1]) {
        check_statement(node->children[1]);
    }

    /* Sai do escopo */
    pop_scope(sym_stack);
}

/*
 * analyze_semantics — Ponto de entrada público
 * -----------------------------------------------------------------------
 * Inicializa a tabela de símbolos vazia e dispara o check_block na raiz.
 */
int analyze_semantics(ASTNode *root) {
    if (!root) return 0;
    sym_stack = init_symbol_table_stack();

    if (root->type == NODE_PROGRAM && root->num_children > 0) {
        check_block(root->children[0]);
    } else if (root->type == NODE_BLOCK) {
        check_block(root);
    }

    /* Limpeza final */
    free_symbol_table_stack(sym_stack);
    sym_stack = NULL;

    return 0;
}
