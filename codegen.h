#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include "ast.h"
#include "symbol_table.h"

void generate_code(ASTNode *root, FILE *out);

#endif
