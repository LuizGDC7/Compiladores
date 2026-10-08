#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

typedef struct Symbol {
    char *name;
    DataType type;
    int line;
    int scope_depth;
    int id;               /* Unique ID per declared symbol instance */
    int stack_offset;     /* Offset for code generation frame */
    struct Symbol *next;
} Symbol;

typedef struct Scope {
    Symbol *symbols;
    int depth;
    struct Scope *parent;
} Scope;

typedef struct SymbolTableStack {
    Scope *top;
    int current_depth;
    int total_symbols;
} SymbolTableStack;

/* Operations as specified in Section 5 */
SymbolTableStack* init_symbol_table_stack(void);
void push_scope(SymbolTableStack *stack);
void pop_scope(SymbolTableStack *stack);
Symbol* lookup_symbol(SymbolTableStack *stack, const char *name);
Symbol* lookup_symbol_current_scope(SymbolTableStack *stack, const char *name);
Symbol* insert_symbol(SymbolTableStack *stack, const char *name, DataType type, int line);
void free_symbol_table_stack(SymbolTableStack *stack);

#endif
