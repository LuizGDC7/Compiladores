#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"
#include "symbol_table.h"

static SymbolTableStack *sym_stack = NULL;

/*
 * [G-V2] Função em análise. Usada pelo comando retorne para comparar o tipo da
 * expressão com o tipo da função. Vale NULL dentro do bloco principal.
 */
static Symbol *current_function = NULL;

/* Mantém a regra do trabalho: reporta o erro e a linha e termina a compilação. */
static void semantic_error(const char *msg, int line) {
    printf("ERRO: %s %d\n", msg, line);
    exit(1);
}

static DataType check_expression(ASTNode *node);
static void check_statement(ASTNode *node);
static void check_block(ASTNode *node);
static void process_declarations(ASTNode *decl_list);

/*
 * [G-V2] Resolve um nome usado como variável (simples ou vetor).
 * Além de exigir a declaração, impede usar o nome de uma função como variável.
 */
static Symbol* resolve_variable(ASTNode *node) {
    Symbol *sym = lookup_symbol(sym_stack, node->string_val);
    if (!sym) {
        semantic_error("VARIAVEL NAO DECLARADA", node->line);
    }
    if (sym->kind == SYM_FUNC) {
        semantic_error("FUNCAO USADA COMO VARIAVEL", node->line);
    }
    return sym;
}

/*
 * [G-V2] Checa um l-value: "x" tem o tipo declarado; "v[i]" exige que v seja um
 * vetor e que o índice seja int, e tem como resultado o tipo básico do vetor.
 */
static DataType check_lvalue(ASTNode *node) {
    Symbol *sym = resolve_variable(node);

    if (node->type == NODE_ARRAY_ACCESS) {
        if (!is_vector_type(sym->type)) {
            semantic_error("INDEXACAO DE VARIAVEL QUE NAO E VETOR", node->line);
        }
        if (check_expression(node->children[0]) != TYPE_INT) {
            semantic_error("INDICE DE VETOR DEVE SER INT", node->line);
        }
        node->data_type = (sym->type == TYPE_CAR_VET) ? TYPE_CAR : TYPE_INT;
    } else {
        node->data_type = sym->type;
    }
    return node->data_type;
}

/*
 * [G-V2] Checa uma chamada: o nome deve ser de função, a quantidade de argumentos
 * deve ser igual à de parâmetros e cada tipo deve ser idêntico ao do parâmetro.
 * Como vetores têm tipo próprio, a comparação direta cobre todos os casos:
 *   - v[i] (int) só casa com parâmetro simples int;
 *   - v (vetor de int) só casa com parâmetro x[]:int.
 */
static DataType check_call(ASTNode *node) {
    Symbol *func = lookup_symbol(sym_stack, node->string_val);
    if (!func) {
        semantic_error("FUNCAO NAO DECLARADA", node->line);
    }
    if (func->kind != SYM_FUNC) {
        semantic_error("IDENTIFICADOR NAO E FUNCAO", node->line);
    }
    if (node->num_children != func->num_params) {
        semantic_error("NUMERO DE ARGUMENTOS INCORRETO NA CHAMADA", node->line);
    }

    ParamInfo *param = func->params;
    for (int i = 0; i < node->num_children; i++) {
        if (check_expression(node->children[i]) != param->type) {
            semantic_error("TIPO DE ARGUMENTO INCOMPATIVEL NA CHAMADA", node->children[i]->line);
        }
        param = param->next;
    }

    node->data_type = func->type;
    return func->type;
}

static DataType check_expression(ASTNode *node) {
    if (!node) return TYPE_NONE;

    switch (node->type) {
        case NODE_INT_CONST:
            node->data_type = TYPE_INT;
            return TYPE_INT;

        case NODE_CAR_CONST:
            node->data_type = TYPE_CAR;
            return TYPE_CAR;

        /* [G-V2] Variáveis e elementos de vetor compartilham a mesma checagem. */
        case NODE_VAR:
        case NODE_ARRAY_ACCESS:
            return check_lvalue(node);

        case NODE_CALL: /* [G-V2] */
            return check_call(node);

        case NODE_ASSIGN: {
            /*
             * [G-V2] O lado esquerdo pode ser variável, elemento ou vetor inteiro.
             * A igualdade de tipos também valida "Y = X" entre vetores, permitida
             * somente quando ambos têm o mesmo tipo básico.
             */
            DataType lhs_type = check_lvalue(node->children[0]);
            DataType rhs_type = check_expression(node->children[1]);
            if (lhs_type != rhs_type) {
                semantic_error("TIPOS INCOMPATIVEIS NA ATRIBUICAO", node->line);
            }
            node->data_type = lhs_type;
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

/*
 * [G-V2] Insere as variáveis de uma seção no escopo atual. Se o nome já existe
 * nesse escopo, diferencia o conflito com parâmetro formal (regra da Seção 6).
 */
static void process_declarations(ASTNode *decl_list) {
    if (!decl_list) return;
    for (int i = 0; i < decl_list->num_children; i++) {
        ASTNode *decl = decl_list->children[i];
        if (decl->type == NODE_VAR_DECL) {
            const char *var_name = decl->string_val;
            int line = decl->line;

            Symbol *existing = lookup_symbol_current_scope(sym_stack, var_name);
            if (existing) {
                semantic_error(existing->kind == SYM_PARAM
                                   ? "VARIAVEL COM MESMO NOME DE PARAMETRO NO MESMO ESCOPO"
                                   : "VARIAVEL JA DECLARADA",
                               line);
            }
            if (decl->is_vector && decl->array_size <= 0) {
                semantic_error("TAMANHO DE VETOR DEVE SER POSITIVO", line);
            }
            insert_variable(sym_stack, var_name, decl->data_type, i + 1, line);
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

        /* [G-V2] O tipo retornado deve ser o tipo da função. */
        case NODE_CMD_RETURN: {
            DataType t = check_expression(node->children[0]);
            if (current_function) {
                if (t != current_function->type) {
                    semantic_error("TIPO DE RETORNO INCOMPATIVEL COM A FUNCAO", node->line);
                }
            } else if (is_vector_type(t)) {
                /* principal não tem tipo declarado, mas não pode retornar vetor. */
                semantic_error("RETORNO DE VETOR NAO PERMITIDO", node->line);
            }
            break;
        }

        /* [G-V2] leia aceita variável simples ou elemento, nunca o vetor inteiro. */
        case NODE_CMD_LEIA: {
            DataType t = check_lvalue(node->children[0]);
            if (is_vector_type(t)) {
                semantic_error("LEIA NAO ACEITA VETOR SEM INDICE", node->line);
            }
            node->data_type = t;
            break;
        }

        /* [G-V2] escreva também exige um valor de tipo básico. */
        case NODE_CMD_ESCREVA_EXPR:
            if (node->num_children > 0 &&
                is_vector_type(check_expression(node->children[0]))) {
                semantic_error("ESCREVA NAO ACEITA VETOR SEM INDICE", node->line);
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

/*
 * [G-V2] Identifica o bloco mais externo de uma função.
 * Os testes do professor usam a forma "{ [ decl ] { cmds } }": um par de chaves
 * cujo único conteúdo é um bloco com declarações. Nesse caso o bloco interno é
 * tratado como o bloco da função, e redeclarar um parâmetro nele é erro
 * (teste fatorial-variávelComMesmoNomeEescopoDeParametroLin9).
 * Se houver outros comandos ao redor, o bloco é realmente aninhado e pode
 * redeclarar o parâmetro, como no exemplo "permitido" do enunciado.
 */
static ASTNode* function_outer_block(ASTNode *body) {
    ASTNode *decls = body->children[0];
    ASTNode *cmds = body->children[1];

    if (decls->num_children == 0 && cmds->num_children == 1 &&
        cmds->children[0]->type == NODE_BLOCK &&
        cmds->children[0]->children[0]->num_children > 0) {
        return cmds->children[0];
    }
    return body;
}

/*
 * [G-V2] Analisa uma função. A entrada é inserida ANTES do corpo para permitir
 * recursão, e somente depois das funções anteriores, o que garante a regra de
 * "declarar antes de usar" (não existem protótipos em G-V2).
 */
static void check_function(ASTNode *func) {
    ASTNode *params = func->children[0];
    ASTNode *body = func->children[1];

    if (lookup_symbol_current_scope(sym_stack, func->string_val)) {
        semantic_error("FUNCAO COM NOME JA DECLARADO", func->line);
    }

    Symbol *func_sym = insert_function(sym_stack, func->string_val, params->num_children,
                                       func->data_type, func->line);

    /* Os parâmetros ficam no mesmo escopo do bloco mais externo da função. */
    push_scope(sym_stack);
    for (int i = 0; i < params->num_children; i++) {
        ASTNode *param = params->children[i];
        if (lookup_symbol_current_scope(sym_stack, param->string_val)) {
            semantic_error("PARAMETRO JA DECLARADO", param->line);
        }
        insert_parameter(sym_stack, param->string_val, param->data_type, i + 1, param->line);
        add_function_param(func_sym, param->string_val, param->data_type, i + 1);
    }

    current_function = func_sym;
    ASTNode *outer = function_outer_block(body);
    process_declarations(outer->children[0]);
    check_statement(outer->children[1]);
    current_function = NULL;

    pop_scope(sym_stack);
}

/*
 * [G-V2] Percorre o programa na ordem do texto: globais, funções e principal.
 * O escopo global fica na base da pilha durante toda a análise.
 */
int analyze_semantics(ASTNode *root) {
    if (!root || root->type != NODE_PROGRAM) return 0;
    sym_stack = init_symbol_table_stack();

    push_scope(sym_stack);
    process_declarations(root->children[0]);

    ASTNode *functions = root->children[1];
    for (int i = 0; i < functions->num_children; i++) {
        check_function(functions->children[i]);
    }

    check_block(root->children[2]);

    free_symbol_table_stack(sym_stack);
    sym_stack = NULL;

    return 0;
}
