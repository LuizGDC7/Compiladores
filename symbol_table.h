#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* [G-V2] Uma entrada da tabela pode ser variável, parâmetro ou função. */
typedef enum {
    SYM_VAR,
    SYM_PARAM,
    SYM_FUNC
} SymbolKind;

/*
 * [G-V2] "Esquema especial" pedido na Seção 5(e) do enunciado.
 * Os parâmetros precisam ser vistos em dois lugares diferentes:
 *   - dentro da função, como variáveis locais (entradas SYM_PARAM no escopo dela);
 *   - em uma chamada externa, para checar os tipos dos argumentos.
 * Esta lista fica pendurada na entrada da FUNÇÃO. Como não está em nenhum escopo,
 * lookup_symbol nunca a encontra: o código externo não enxerga os nomes, mas o
 * compilador consegue consultar quantidade, ordem e tipos em cada chamada.
 */
typedef struct ParamInfo {
    char *name;
    DataType type;
    int position;           /* posição na lista de parâmetros (a partir de 1) */
    struct ParamInfo *next;
} ParamInfo;

typedef struct Symbol {
    char *name;
    SymbolKind kind;        /* [G-V2] variável, parâmetro ou função */
    DataType type;          /* tipo da variável/parâmetro ou retorno da função */
    int line;
    int scope_depth;
    int position;           /* [G-V2] posição na lista de declarações/parâmetros */
    int is_global;          /* [G-V2] 1 se está no escopo global (acesso por rótulo) */
    int stack_offset;       /* Deslocamento relativo a $fp na geração de código */
    int num_params;         /* [G-V2] apenas funções */
    ParamInfo *params;      /* [G-V2] apenas funções */
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

/* Operações da Seção 5 do enunciado do G-V2 */
SymbolTableStack* init_symbol_table_stack(void);                /* (a) */
void push_scope(SymbolTableStack *stack);                       /* (b) */
Symbol* lookup_symbol(SymbolTableStack *stack, const char *name); /* (c) */
void pop_scope(SymbolTableStack *stack);                        /* (d) */
/* (e) [G-V2] insere função */
Symbol* insert_function(SymbolTableStack *stack, const char *name, int num_params,
                        DataType return_type, int line);
/* (e) [G-V2] registra um parâmetro no "esquema especial" da função */
void add_function_param(Symbol *func, const char *name, DataType type, int position);
/* (f) [G-V2] insere variável */
Symbol* insert_variable(SymbolTableStack *stack, const char *name, DataType type,
                        int position, int line);
/* (g) [G-V2] insere parâmetro no escopo da função */
Symbol* insert_parameter(SymbolTableStack *stack, const char *name, DataType type,
                         int position, int line);
void free_symbol_table_stack(SymbolTableStack *stack);          /* (h) */

/* Busca somente no escopo do topo: usada para detectar redeclarações. */
Symbol* lookup_symbol_current_scope(SymbolTableStack *stack, const char *name);

#endif
