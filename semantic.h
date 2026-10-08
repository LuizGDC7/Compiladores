/*
 * =============================================================================
 * semantic.h — Análise Semântica (Header)
 * =============================================================================
 *
 * RESPONSABILIDADE:
 *   Declarar a função principal de análise semântica.
 *   Esta fase do compilador verifica as "regras de significado" do código
 *   (que não podem ser expressas apenas pela gramática), como:
 *   - Uso de variáveis não declaradas
 *   - Declaração de múltiplas variáveis com o mesmo nome no mesmo escopo
 *   - Compatibilidade de tipos em operações (ex: somar inteiro com inteiro)
 *
 * RELAÇÃO COM OUTROS ARQUIVOS:
 *   → ast.h: recebe a AST gerada pelo parser.
 *   → symbol_table.h: usa a tabela de símbolos para rastrear variáveis.
 *   → main.c: chama `analyze_semantics` após o parsing.
 * =============================================================================
 */

#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include "symbol_table.h"

/*
 * analyze_semantics — Realiza a análise semântica na AST
 * -----------------------------------------------------------------------
 * Percorre a árvore sintática abstrata (AST) a partir da raiz, realizando
 * a checagem de tipos e gerenciamento de escopo/tabela de símbolos.
 *
 * Parâmetros:
 *   root → ponteiro para o nó raiz da AST (geralmente NODE_PROGRAM).
 *
 * Retorna: 
 *   0 se a análise semântica foi concluída com sucesso (sem erros).
 *   (Na prática, em caso de erro, a função interna `semantic_error` chama
 *   `exit(1)`, abortando o programa).
 */
int analyze_semantics(ASTNode *root);

#endif
