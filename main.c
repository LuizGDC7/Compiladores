/*
 * =============================================================================
 * main.c — Ponto de Entrada do Compilador
 * =============================================================================
 *
 * RESPONSABILIDADE:
 *   Agrupar o fluxo principal do compilador (A Pipeline de Compilação).
 *   O programa percorre ordenadamente as seguintes fases:
 *
 *   1. Arquivo G-V1 lido da linha de comando
 *   2. Análise Sintática / Léxica (yyparse) - Parser e Lexer
 *   3. Análise Semântica (analyze_semantics)
 *   4. Geração de Código (generate_code)
 *
 * FLUXO DE ERROS:
 *   Se qualquer etapa falhar, o programa imprime o erro no STDERR (já implementado
 *   nas funções de cada fase) e encerra o programa (return 1).
 * =============================================================================
 */

#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "semantic.h"
#include "codegen.h"

/* Variável global do Flex (lexer) para indicar de qual arquivo ele deve ler a string */
extern FILE *yyin;

/* Função do Bison (parser) que processa as palavras recebidas do lexer e constrói a árvore */
extern int yyparse(void);

/* Raiz da Árvore Sintática Abstrata, preenchida pelo `yyparse()` no arquivo g-v1.y */
extern ASTNode *ast_root;

int main(int argc, char **argv) {
    /* 1. Validação dos Argumentos de Linha de Comando */
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo_fonte>\n", argv[0]);
        return 1;
    }

    /* 2. Abertura do Arquivo Fonte (.g) */
    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", argv[1]);
        return 1;
    }

    /* 3. Prepara o Lexer (yyin define onde o scanner deve fazer a leitura) */
    yyin = fp;

    /* 4. Análise Sintática & Léxica (Bison & Flex) */
    /* Isso lê o código fonte, avalia a gramática, e cria a AST em ast_root */
    int parse_result = yyparse();
    fclose(fp);

    /* yyparse() retorna 0 se não houve erros de sintaxe */
    if (parse_result != 0) {
        return 1;
    }

    /* Garantia que a raiz foi preenchida pelas regras de gramática do Bison */
    if (!ast_root) {
        fprintf(stderr, "Erro: Árvore sintática abstrata não gerada.\n");
        return 1;
    }

    /* 5. Análise Semântica */
    /* Checa tipos e se variáveis foram declaradas/re-declaradas (regras lógicas) */
    if (analyze_semantics(ast_root) != 0) {
        free_ast(ast_root);
        return 1;
    }

    /* 6. Geração de Código */
    /* Traduz para Assembly imprimindo direto no Standard Output (stdout). */
    /* Por isso você roda "./g-v1 arquivo.g > saida.asm" no terminal. */
    generate_code(ast_root, stdout);

    /* Limpa toda a AST da memória para evitar Memory Leaks */
    free_ast(ast_root);
    return 0;
}
