#define _DEFAULT_SOURCE
#include "symbol_table.h"

SymbolTableStack* init_symbol_table_stack(void) {
    SymbolTableStack *stack = (SymbolTableStack*) malloc(sizeof(SymbolTableStack));
    if (!stack) {
        fprintf(stderr, "Erro de alocação para pilha de tabela de símbolos\n");
        exit(1);
    }
    stack->top = NULL;
    stack->current_depth = 0;
    stack->total_symbols = 0;
    return stack;
}

void push_scope(SymbolTableStack *stack) {
    if (!stack) return;
    Scope *new_scope = (Scope*) malloc(sizeof(Scope));
    if (!new_scope) {
        fprintf(stderr, "Erro de alocação para novo escopo\n");
        exit(1);
    }
    stack->current_depth++;
    new_scope->symbols = NULL;
    new_scope->depth = stack->current_depth;
    new_scope->parent = stack->top;
    stack->top = new_scope;
}

void pop_scope(SymbolTableStack *stack) {
    if (!stack || !stack->top) return;
    Scope *old_scope = stack->top;
    stack->top = old_scope->parent;
    stack->current_depth--;

    Symbol *curr = old_scope->symbols;
    while (curr) {
        Symbol *next = curr->next;
        if (curr->name) free(curr->name);
        free(curr);
        curr = next;
    }
    free(old_scope);
}

Symbol* lookup_symbol(SymbolTableStack *stack, const char *name) {
    if (!stack || !name) return NULL;
    Scope *scope = stack->top;
    while (scope) {
        Symbol *sym = scope->symbols;
        while (sym) {
            if (strcmp(sym->name, name) == 0) {
                return sym;
            }
            sym = sym->next;
        }
        scope = scope->parent;
    }
    return NULL;
}

Symbol* lookup_symbol_current_scope(SymbolTableStack *stack, const char *name) {
    if (!stack || !stack->top || !name) return NULL;
    Symbol *sym = stack->top->symbols;
    while (sym) {
        if (strcmp(sym->name, name) == 0) {
            return sym;
        }
        sym = sym->next;
    }
    return NULL;
}

Symbol* insert_symbol(SymbolTableStack *stack, const char *name, DataType type, int line) {
    if (!stack || !stack->top || !name) return NULL;

    Symbol *sym = (Symbol*) malloc(sizeof(Symbol));
    if (!sym) {
        fprintf(stderr, "Erro de alocação para símbolo\n");
        exit(1);
    }
    stack->total_symbols++;
    sym->name = strdup(name);
    sym->type = type;
    sym->line = line;
    sym->scope_depth = stack->top->depth;
    sym->id = stack->total_symbols;
    sym->stack_offset = stack->total_symbols * 4;

    sym->next = stack->top->symbols;
    stack->top->symbols = sym;

    return sym;
}

void free_symbol_table_stack(SymbolTableStack *stack) {
    if (!stack) return;
    while (stack->top) {
        pop_scope(stack);
    }
    free(stack);
}
