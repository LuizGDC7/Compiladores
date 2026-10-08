/*
 * =============================================================================
 * symbol_table.c — Implementação da Tabela de Símbolos
 * =============================================================================
 *
 * RESPONSABILIDADE:
 *   Implementa todas as operações sobre a pilha de escopos. Gerencia alocação
 *   e liberação de memória dos escopos e símbolos.
 *
 * ALGORITMO DE BUSCA (lookup_symbol):
 *   Percorre a pilha de cima (escopo atual) para baixo (escopo global).
 *   Isso implementa a regra de "escopo léxico": uma variável interna
 *   "sombra" uma variável externa de mesmo nome.
 *
 * ALGORITMO DE ID ÚNICO (insert_symbol):
 *   O campo total_symbols é um contador global crescente. Cada símbolo
 *   recebe um ID = total_symbols incrementado. Esses IDs são usados pelo
 *   codegen para nomear variáveis no Assembly: var_1, var_2, etc.
 *   Como o contador nunca decresce, IDs são únicos mesmo entre escopos
 *   diferentes que tenham sido destruídos e recriados.
 * =============================================================================
 */

#define _DEFAULT_SOURCE
#include "symbol_table.h"

/*
 * init_symbol_table_stack — Cria uma tabela de símbolos vazia
 * -----------------------------------------------------------------------
 * Aloca a estrutura SymbolTableStack e inicializa:
 *   - top = NULL          → ainda não há escopos
 *   - current_depth = 0   → profundidade zero (fora de qualquer bloco)
 *   - total_symbols = 0   → nenhum símbolo inserido ainda
 *
 * Deve ser chamada UMA VEZ antes de iniciar a análise.
 * Resultado: estrutura pronta para receber push_scope().
 */
SymbolTableStack* init_symbol_table_stack(void) {
    SymbolTableStack *stack = (SymbolTableStack*) malloc(sizeof(SymbolTableStack));
    if (!stack) {
        fprintf(stderr, "Erro de alocação para pilha de tabela de símbolos\n");
        exit(1);
    }
    stack->top           = NULL;
    stack->current_depth = 0;
    stack->total_symbols = 0;
    return stack;
}

/*
 * push_scope — Empilha um novo escopo (entra em um bloco)
 * -----------------------------------------------------------------------
 * Chamada sempre que o compilador entra em um bloco { }.
 * Cria um novo Scope, define seu pai como o escopo atual, e o coloca
 * no topo da pilha. A profundidade é incrementada.
 *
 * Exemplo de sequência para "principal { } { se (...) entao { } fimse }":
 *   push_scope() → depth=1 (bloco principal)
 *   push_scope() → depth=2 (bloco do "se")
 *   pop_scope()  → depth volta a 1
 *   pop_scope()  → depth volta a 0
 */
void push_scope(SymbolTableStack *stack) {
    if (!stack) return;
    Scope *new_scope = (Scope*) malloc(sizeof(Scope));
    if (!new_scope) {
        fprintf(stderr, "Erro de alocação para novo escopo\n");
        exit(1);
    }
    stack->current_depth++;
    new_scope->symbols = NULL;                /* ainda sem símbolos neste escopo */
    new_scope->depth   = stack->current_depth;
    new_scope->parent  = stack->top;          /* encadeia com o escopo anterior */
    stack->top         = new_scope;           /* este passa a ser o escopo atual */
}

/*
 * pop_scope — Desempilha o escopo atual (sai de um bloco)
 * -----------------------------------------------------------------------
 * Chamada ao fechar um bloco }. Remove o escopo do topo e libera a
 * memória de TODOS os símbolos declarados nele.
 *
 * Após o pop, as variáveis daquele escopo deixam de existir — qualquer
 * referência a elas após esse ponto seria um erro semântico.
 *
 * Memória liberada: cada Symbol (name + struct) e o próprio Scope.
 */
void pop_scope(SymbolTableStack *stack) {
    if (!stack || !stack->top) return;
    Scope *old_scope = stack->top;
    stack->top = old_scope->parent;  /* restaura o escopo pai como atual */
    stack->current_depth--;

    /* percorre a lista encadeada de símbolos e libera cada um */
    Symbol *curr = old_scope->symbols;
    while (curr) {
        Symbol *next = curr->next;
        if (curr->name) free(curr->name);
        free(curr);
        curr = next;
    }
    free(old_scope);
}

/*
 * lookup_symbol — Busca um símbolo em todos os escopos visíveis
 * -----------------------------------------------------------------------
 * Percorre a pilha de escopos do TOPO (escopo atual) para a BASE (escopo
 * global), verificando a lista de símbolos de cada escopo.
 *
 * Isso implementa a regra de visibilidade léxica: uma variável declarada
 * em um escopo interno é encontrada antes da mesma variável num escopo externo.
 *
 * Retorna: ponteiro para o Symbol encontrado, ou NULL se não existir.
 *
 * Uso típico em semantic.c:
 *   Symbol *s = lookup_symbol(sym_stack, "x");
 *   if (!s) → erro "VARIAVEL NAO DECLARADA"
 */
Symbol* lookup_symbol(SymbolTableStack *stack, const char *name) {
    if (!stack || !name) return NULL;
    Scope *scope = stack->top;
    while (scope) {                          /* percorre escopos da pilha */
        Symbol *sym = scope->symbols;
        while (sym) {                        /* percorre símbolos do escopo */
            if (strcmp(sym->name, name) == 0) {
                return sym;                  /* encontrou! */
            }
            sym = sym->next;
        }
        scope = scope->parent;               /* sobe para o escopo pai */
    }
    return NULL;                             /* não encontrou em nenhum escopo */
}

/*
 * lookup_symbol_current_scope — Busca símbolo APENAS no escopo atual
 * -----------------------------------------------------------------------
 * Diferente de lookup_symbol(), esta versão NÃO sobe para escopos externos.
 * Usada exclusivamente para detectar REDECLARAÇÃO no mesmo bloco.
 *
 * Exemplo: se "x: int;" aparece duas vezes no mesmo bloco, a segunda
 * chamada a insert_symbol() chama esta função primeiro e retorna um Symbol
 * não-NULL, indicando que "x" já foi declarada neste escopo.
 *
 * Retorna: Symbol se já declarado no escopo atual, NULL caso contrário.
 */
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
 * insert_symbol — Insere um novo símbolo no escopo atual
 * -----------------------------------------------------------------------
 * Cria um Symbol com as informações da variável declarada e o adiciona
 * no INÍCIO da lista encadeada do escopo atual (inserção no topo = O(1)).
 *
 * O campo "id" recebe um valor único e crescente (total_symbols++).
 * Esse ID é fundamental para o codegen, que gera rótulos "var_ID" no
 * segmento .data do Assembly MIPS.
 *
 * Parâmetros:
 *   name → nome da variável (será copiado com strdup)
 *   type → TYPE_INT ou TYPE_CAR
 *   line → linha da declaração (para mensagens de erro futuras)
 *
 * Retorna: ponteiro para o Symbol inserido (usado pelo codegen para pegar o ID).
 *
 * Exemplo de uso em semantic.c:
 *   insert_symbol(sym_stack, "contador", TYPE_INT, 5);
 *   → cria var_1 (ou var_N) no Assembly
 */
Symbol* insert_symbol(SymbolTableStack *stack, const char *name, DataType type, int line) {
    if (!stack || !stack->top || !name) return NULL;

    Symbol *sym = (Symbol*) malloc(sizeof(Symbol));
    if (!sym) {
        fprintf(stderr, "Erro de alocação para símbolo\n");
        exit(1);
    }
    stack->total_symbols++;           /* ID global único; não reutilizado */
    sym->name        = strdup(name);
    sym->type        = type;
    sym->line        = line;
    sym->scope_depth = stack->top->depth;
    sym->id          = stack->total_symbols;         /* ex: 1, 2, 3... */
    sym->stack_offset = stack->total_symbols * 4;   /* 4 bytes por variável (word) */

    /* insere no início da lista do escopo atual */
    sym->next            = stack->top->symbols;
    stack->top->symbols  = sym;

    return sym;
}

/*
 * free_symbol_table_stack — Libera toda a memória da tabela de símbolos
 * -----------------------------------------------------------------------
 * Faz pop_scope() repetidamente até esvaziar a pilha (cada pop libera
 * os símbolos daquele escopo), depois libera o próprio stack.
 *
 * Chamada no final de analyze_semantics() e generate_code() para garantir
 * que não haja vazamento de memória.
 */
void free_symbol_table_stack(SymbolTableStack *stack) {
    if (!stack) return;
    while (stack->top) {
        pop_scope(stack);    /* cada pop libera o escopo e seus símbolos */
    }
    free(stack);
}
