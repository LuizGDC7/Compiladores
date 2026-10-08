/*
 * =============================================================================
 * symbol_table.h — Tabela de Símbolos com Suporte a Escopos
 * =============================================================================
 *
 * RESPONSABILIDADE:
 *   Define as estruturas e operações da tabela de símbolos, que é o mecanismo
 *   usado para rastrear todas as variáveis declaradas no programa, seus tipos,
 *   e em qual escopo (bloco) elas existem.
 *
 * O QUE É UM SÍMBOLO?
 *   Um símbolo representa uma variável declarada. Ex: "x: int;" cria um
 *   símbolo com nome="x", type=TYPE_INT.
 *
 * O QUE É UM ESCOPO?
 *   Cada bloco { } cria um novo escopo. Variáveis declaradas dentro de um
 *   bloco aninhado existem apenas naquele bloco e são destruídas ao sair.
 *   Exemplo:
 *
 *     principal {
 *         x: int;    ← escopo externo (profundidade 1)
 *     } {
 *         se (x > 0) entao {
 *             y: int;    ← escopo interno (profundidade 2); y só existe aqui
 *         } fimse
 *     }
 *
 * ESTRUTURA DE DADOS: PILHA DE ESCOPOS
 *   A tabela é implementada como uma PILHA de escopos:
 *   - "entrar em um bloco" → push_scope() empilha um novo escopo
 *   - "sair de um bloco"   → pop_scope() desempilha e libera o escopo
 *   - "buscar variável"    → lookup_symbol() percorre da pilha de cima para baixo
 *
 *   Pilha (top → base):
 *   ┌──────────────────┐
 *   │  Escopo 2 (y)    │  ← topo (escopo atual)
 *   ├──────────────────┤
 *   │  Escopo 1 (x)    │  ← pai
 *   └──────────────────┘
 *
 * RELAÇÃO COM OUTROS ARQUIVOS:
 *   → semantic.c   usa para verificar declarações e tipos
 *   → codegen.c    usa para mapear variáveis a endereços Assembly (var_ID)
 * =============================================================================
 */

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* ---------------------------------------------------------------------------
 * Symbol — Representa uma variável declarada
 * ---------------------------------------------------------------------------
 * Cada declaração de variável ("nome: tipo;") cria um Symbol. Os Symbols
 * de um mesmo escopo formam uma lista encadeada simples.
 *
 * Campos:
 *   name         → nome da variável (cópia do texto original, ex: "contador")
 *   type         → tipo da variável: TYPE_INT ou TYPE_CAR
 *   line         → linha onde foi declarada (para mensagens de erro)
 *   scope_depth  → profundidade do escopo onde foi declarada (1 = mais externo)
 *   id           → ID único e crescente para cada símbolo no programa;
 *                  usado pelo codegen para gerar rótulos únicos "var_ID" no Assembly
 *   stack_offset → deslocamento no frame de pilha (id * 4 bytes);
 *                  reservado para uso futuro com alocação em memória de pilha
 *   next         → próximo símbolo na lista encadeada do mesmo escopo
 * --------------------------------------------------------------------------- */
typedef struct Symbol {
    char    *name;
    DataType type;
    int      line;
    int      scope_depth;
    int      id;            /* identificador único; gera rótulos como "var_1", "var_2" */
    int      stack_offset;  /* offset em bytes para uso em frames de pilha */
    struct Symbol *next;    /* encadeamento dentro do mesmo escopo */
} Symbol;

/* ---------------------------------------------------------------------------
 * Scope — Representa um bloco/escopo ativo
 * ---------------------------------------------------------------------------
 * Cada vez que o programa entra em um bloco { }, um novo Scope é criado
 * e empilhado. Cada Scope mantém a lista de símbolos declarados nele.
 *
 * Campos:
 *   symbols → lista encadeada de Symbol declarados neste escopo
 *   depth   → nível de aninhamento (1 = escopo do programa principal)
 *   parent  → ponteiro para o escopo externo (abaixo na pilha);
 *             permite busca hierárquica em lookup_symbol()
 * --------------------------------------------------------------------------- */
typedef struct Scope {
    Symbol      *symbols;  /* lista de variáveis deste escopo */
    int          depth;    /* profundidade: 1, 2, 3... */
    struct Scope *parent;  /* escopo que envolve este; NULL no escopo raiz */
} Scope;

/* ---------------------------------------------------------------------------
 * SymbolTableStack — Pilha de escopos (a tabela de símbolos em si)
 * ---------------------------------------------------------------------------
 * É a estrutura principal. Mantém o topo da pilha e contadores globais.
 *
 * Campos:
 *   top            → ponteiro para o escopo atual (topo da pilha)
 *   current_depth  → profundidade do escopo atual (igual a top->depth)
 *   total_symbols  → contador global de símbolos inseridos; garante IDs únicos
 *                    mesmo após pop_scope() (IDs nunca são reutilizados)
 * --------------------------------------------------------------------------- */
typedef struct SymbolTableStack {
    Scope *top;
    int    current_depth;
    int    total_symbols;  /* monotonicamente crescente; nunca decresce */
} SymbolTableStack;

/* ---------------------------------------------------------------------------
 * Funções de operação sobre a tabela de símbolos (implementadas em symbol_table.c)
 * ---------------------------------------------------------------------------
 *
 * init_symbol_table_stack()
 *   Aloca e retorna uma nova tabela vazia (sem escopos).
 *   Chamada no início de analyze_semantics() e generate_code().
 *
 * push_scope(stack)
 *   Cria um novo escopo e o empilha sobre o atual.
 *   Chamada toda vez que o compilador entra em um bloco { }.
 *
 * pop_scope(stack)
 *   Remove o escopo do topo, liberando todos os símbolos dele.
 *   Chamada ao sair de um bloco { }. Variáveis locais deixam de existir.
 *
 * lookup_symbol(stack, name)
 *   Busca "name" do escopo atual até o escopo mais externo (varredura da pilha).
 *   Retorna o Symbol encontrado, ou NULL se não existir.
 *   Usada para verificar uso de variável não declarada.
 *
 * lookup_symbol_current_scope(stack, name)
 *   Busca "name" APENAS no escopo atual (topo da pilha).
 *   Usada para detectar redeclaração no mesmo escopo ("VARIAVEL JA DECLARADA").
 *
 * insert_symbol(stack, name, type, line)
 *   Insere um novo símbolo no escopo atual. Atribui um ID único.
 *   Chamada ao processar cada declaração de variável.
 *   Retorna o Symbol criado (usado pelo codegen para obter o ID).
 *
 * free_symbol_table_stack(stack)
 *   Libera todos os escopos e símbolos restantes, então libera o stack.
 *   Chamada no final de analyze_semantics() e generate_code().
 * --------------------------------------------------------------------------- */
SymbolTableStack* init_symbol_table_stack(void);
void    push_scope(SymbolTableStack *stack);
void    pop_scope(SymbolTableStack *stack);
Symbol* lookup_symbol(SymbolTableStack *stack, const char *name);
Symbol* lookup_symbol_current_scope(SymbolTableStack *stack, const char *name);
Symbol* insert_symbol(SymbolTableStack *stack, const char *name, DataType type, int line);
void    free_symbol_table_stack(SymbolTableStack *stack);

#endif
