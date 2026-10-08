/*
 * =============================================================================
 * codegen.h — Geração de Código (Header)
 * =============================================================================
 *
 * RESPONSABILIDADE:
 *   Declarar a função que traduz a AST validada para código de máquina (Assembly MIPS).
 *   Esta é a última fase principal do compilador (Back-end).
 *
 * RELAÇÃO COM OUTROS ARQUIVOS:
 *   → ast.h: recebe a AST final (após análise sintática e semântica).
 *   → symbol_table.h: recria os escopos para referenciar corretamente 
 *                     as variáveis declaradas (os "var_ID").
 *   → main.c: chama `generate_code` imprimindo em `stdout`.
 * =============================================================================
 */

#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include "ast.h"
#include "symbol_table.h"

/*
 * generate_code — Traduz o programa para Assembly
 * -----------------------------------------------------------------------
 * Percorre a AST traduzindo as instruções de alto nível para as instruções
 * de baixo nível em arquitetura MIPS (simulável pelo SPIM ou MARS).
 *
 * Parâmetros:
 *   root → ponteiro para o nó raiz da AST.
 *   out  → arquivo/stream de saída onde o Assembly será escrito (ex: stdout).
 */
void generate_code(ASTNode *root, FILE *out);

#endif
