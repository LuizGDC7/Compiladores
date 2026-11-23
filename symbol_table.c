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
        /* [G-V2] Funções possuem uma lista própria de parâmetros a liberar. */
        ParamInfo *param = curr->params;
        while (param) {
            ParamInfo *next_param = param->next;
            free(param->name);
            free(param);
            param = next_param;
        }
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

/*
 * [G-V2] Rotina comum às inserções (e), (f) e (g). Substitui o antigo
 * insert_symbol e preenche os campos novos da entrada.
 */
static Symbol* insert_entry(SymbolTableStack *stack, const char *name, SymbolKind kind,
                            DataType type, int position, int line) {
    if (!stack || !stack->top || !name) return NULL;

    Symbol *sym = (Symbol*) malloc(sizeof(Symbol));
    if (!sym) {
        fprintf(stderr, "Erro de alocação para símbolo\n");
        exit(1);
    }
    stack->total_symbols++;
    sym->name = strdup(name);
    sym->kind = kind;
    sym->type = type;
    sym->line = line;
    sym->scope_depth = stack->top->depth;
    sym->position = position;
    /* O primeiro escopo empilhado (profundidade 1) é o escopo global. */
    sym->is_global = (stack->top->depth == 1);
    sym->stack_offset = 0;
    sym->num_params = 0;
    sym->params = NULL;

    sym->next = stack->top->symbols;
    stack->top->symbols = sym;

    return sym;
}

/* [G-V2] Seção 5(e): insere o nome de uma função no escopo atual. */
Symbol* insert_function(SymbolTableStack *stack, const char *name, int num_params,
                        DataType return_type, int line) {
    Symbol *sym = insert_entry(stack, name, SYM_FUNC, return_type, 0, line);
    if (sym) sym->num_params = num_params;
    return sym;
}

/*
 * [G-V2] Seção 5(e): acrescenta um parâmetro à lista da função, mantendo a ordem
 * de declaração, que é a ordem usada para comparar com os argumentos da chamada.
 */
void add_function_param(Symbol *func, const char *name, DataType type, int position) {
    if (!func) return;
    ParamInfo *param = (ParamInfo*) malloc(sizeof(ParamInfo));
    if (!param) {
        fprintf(stderr, "Erro de alocação para parâmetro\n");
        exit(1);
    }
    param->name = strdup(name);
    param->type = type;
    param->position = position;
    param->next = NULL;

    if (!func->params) {
        func->params = param;
    } else {
        ParamInfo *last = func->params;
        while (last->next) last = last->next;
        last->next = param;
    }
}

/* [G-V2] Seção 5(f): insere uma variável no escopo atual. */
Symbol* insert_variable(SymbolTableStack *stack, const char *name, DataType type,
                        int position, int line) {
    return insert_entry(stack, name, SYM_VAR, type, position, line);
}

/* [G-V2] Seção 5(g): insere um parâmetro no escopo atual (o escopo da função). */
Symbol* insert_parameter(SymbolTableStack *stack, const char *name, DataType type,
                         int position, int line) {
    return insert_entry(stack, name, SYM_PARAM, type, position, line);
}

void free_symbol_table_stack(SymbolTableStack *stack) {
    if (!stack) return;
    while (stack->top) {
        pop_scope(stack);
    }
    free(stack);
}
