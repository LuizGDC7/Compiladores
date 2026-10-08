#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "semantic.h"
#include "codegen.h"

extern FILE *yyin;
extern int yyparse(void);
extern ASTNode *ast_root;

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo_fonte>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", argv[1]);
        return 1;
    }

    yyin = fp;

    int parse_result = yyparse();
    fclose(fp);

    if (parse_result != 0) {
        return 1;
    }

    if (!ast_root) {
        fprintf(stderr, "Erro: Árvore sintática abstrata não gerada.\n");
        return 1;
    }

    if (analyze_semantics(ast_root) != 0) {
        free_ast(ast_root);
        return 1;
    }

    generate_code(ast_root, stdout);

    free_ast(ast_root);
    return 0;
}
