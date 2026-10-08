/*
 * =============================================================================
 * ast.h — Árvore Sintática Abstrata (AST)
 * =============================================================================
 *
 * RESPONSABILIDADE:
 *   Define as estruturas de dados que representam o programa G-V1 em memória
 *   após a análise sintática. Funciona como a "planta baixa" do código-fonte:
 *   cada construção da linguagem (declarações, comandos, expressões) vira um nó
 *   nessa árvore.
 *
 * POR QUE UMA ÁRVORE?
 *   A árvore permite que as fases seguintes (análise semântica e geração de
 *   código) percorram o programa de forma estruturada e recursiva, sem
 *   precisar reler o arquivo de texto original.
 *
 * FLUXO GERAL:
 *   1. O parser (g-v1.y) cria nós da AST enquanto reconhece a gramática.
 *   2. A análise semântica (semantic.c) percorre a árvore para checar tipos.
 *   3. O gerador de código (codegen.c) percorre a árvore para emitir Assembly.
 *
 * EXEMPLO DE ÁRVORE para "x = a + 2;":
 *
 *   NODE_CMD_EXPR
 *       └── NODE_ASSIGN
 *               ├── NODE_VAR ("x")
 *               └── NODE_BIN_OP (OP_ADD)
 *                       ├── NODE_VAR ("a")
 *                       └── NODE_INT_CONST (2)
 * =============================================================================
 */

#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------------------------------------------------------------------
 * DataType — Tipos de dados da linguagem G-V1
 * ---------------------------------------------------------------------------
 * Representa o tipo de uma variável ou de uma expressão após a análise
 * semântica. Usado para verificar compatibilidade de tipos nas operações.
 *
 *   TYPE_NONE    → tipo ainda não determinado (nó recém-criado)
 *   TYPE_INT     → tipo inteiro, palavra-chave "int" na linguagem
 *   TYPE_CAR     → tipo caractere, palavra-chave "car" na linguagem
 *   TYPE_UNKNOWN → tipo indeterminado / erro interno
 * --------------------------------------------------------------------------- */
typedef enum {
    TYPE_NONE,
    TYPE_INT,
    TYPE_CAR,
    TYPE_UNKNOWN
} DataType;

/* ---------------------------------------------------------------------------
 * NodeType — Categorias de nós da AST
 * ---------------------------------------------------------------------------
 * Cada valor representa um tipo diferente de construção do programa.
 * O campo "type" de cada ASTNode indica qual dessas categorias ele é,
 * permitindo que as funções de traversal usem um switch para tratá-lo.
 *
 *   NODE_PROGRAM          → raiz da árvore; representa o programa inteiro
 *   NODE_BLOCK            → bloco { declarações } { comandos }
 *   NODE_VAR_DECL_LIST    → lista de declarações de variáveis
 *   NODE_VAR_DECL         → declaração de uma única variável (nome + tipo)
 *   NODE_CMD_LIST         → lista de comandos dentro de um bloco
 *   NODE_CMD_EXPR         → comando que é uma expressão (ex: "x = 5;")
 *   NODE_CMD_LEIA         → comando "leia x;" (leitura do stdin)
 *   NODE_CMD_ESCREVA_EXPR → comando "escreva expr;" (imprime valor)
 *   NODE_CMD_ESCREVA_STRING → comando "escreva "texto";" (imprime string)
 *   NODE_CMD_NOVALINHA    → comando "novalinha;" (imprime '\n')
 *   NODE_CMD_IF           → comando "se ... entao ... [senao ...] fimse"
 *   NODE_CMD_WHILE        → comando "enquanto (...) { ... }"
 *   NODE_ASSIGN           → atribuição "x = expr"
 *   NODE_BIN_OP           → operação binária (ex: a + b, a == b)
 *   NODE_UN_OP            → operação unária (ex: -x, !x)
 *   NODE_VAR              → referência a uma variável pelo nome
 *   NODE_INT_CONST        → literal inteiro (ex: 42)
 *   NODE_CAR_CONST        → literal de caractere (ex: 'A')
 *   NODE_STRING_CONST     → literal de string (ex: "hello")
 * --------------------------------------------------------------------------- */
typedef enum {
    NODE_PROGRAM,
    NODE_BLOCK,
    NODE_VAR_DECL_LIST,
    NODE_VAR_DECL,
    NODE_CMD_LIST,
    NODE_CMD_EXPR,
    NODE_CMD_LEIA,
    NODE_CMD_ESCREVA_EXPR,
    NODE_CMD_ESCREVA_STRING,
    NODE_CMD_NOVALINHA,
    NODE_CMD_IF,
    NODE_CMD_WHILE,
    NODE_ASSIGN,
    NODE_BIN_OP,
    NODE_UN_OP,
    NODE_VAR,
    NODE_INT_CONST,
    NODE_CAR_CONST,
    NODE_STRING_CONST
} NodeType;

/* ---------------------------------------------------------------------------
 * OperatorType — Operadores suportados pela linguagem
 * ---------------------------------------------------------------------------
 * Armazenado no campo "int_val" dos nós NODE_BIN_OP e NODE_UN_OP para indicar
 * qual operação deve ser realizada.
 *
 *   Lógicos:     OP_OU (||), OP_E (&)
 *   Igualdade:   OP_IGUAL (==), OP_DIFERENTE (!=)
 *   Relacionais: OP_LESS (<), OP_GREATER (>), OP_GE (>=), OP_LE (<=)
 *   Aritméticos: OP_ADD (+), OP_SUB (-), OP_MUL (*), OP_DIV (/)
 *   Unários:     OP_NEG (negação aritmética: -x), OP_NOT (negação lógica: !x)
 * --------------------------------------------------------------------------- */
typedef enum {
    OP_OU,
    OP_E,
    OP_IGUAL,
    OP_DIFERENTE,
    OP_LESS,
    OP_GREATER,
    OP_GE,
    OP_LE,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_NEG,
    OP_NOT
} OperatorType;

/* Declaração antecipada de Symbol, definido em symbol_table.h.
 * Necessária porque ASTNode referencia Symbol e Symbol pode referenciar AST. */
struct Symbol;

/* ---------------------------------------------------------------------------
 * ASTNode — Nó da Árvore Sintática Abstrata
 * ---------------------------------------------------------------------------
 * Estrutura central do compilador. Cada nó representa UMA construção do
 * programa. Os nós são ligados em árvore pelo array "children".
 *
 * Campos:
 *   type        → qual tipo de nó é (NodeType acima)
 *   line        → linha do código-fonte onde essa construção aparece;
 *                 usado para mensagens de erro semântico com número de linha
 *   data_type   → tipo de dado (int / car); preenchido pela análise semântica
 *   string_val  → usado em: NODE_VAR (nome da variável),
 *                           NODE_STRING_CONST (texto da string),
 *                           NODE_VAR_DECL (nome declarado),
 *                           NODE_CMD_LEIA (nome da variável lida),
 *                           NODE_CMD_ESCREVA_STRING (texto a imprimir)
 *   int_val     → usado em: NODE_INT_CONST (valor inteiro literal),
 *                           NODE_BIN_OP / NODE_UN_OP (código do operador)
 *   char_val    → usado em: NODE_CAR_CONST (valor do caractere literal)
 *   symbol      → ponteiro para a entrada na tabela de símbolos (opcional;
 *                 associado durante a análise semântica para variáveis)
 *   num_children→ quantos filhos este nó tem
 *   children    → array dinâmico de ponteiros para os nós filhos
 * --------------------------------------------------------------------------- */
typedef struct ASTNode {
    NodeType  type;
    int       line;
    DataType  data_type;

    char *string_val;        /* Para identificadores e strings literais */
    int   int_val;           /* Para inteiros literais e código do operador */
    char  char_val;          /* Para caracteres literais */

    struct Symbol *symbol;   /* Entrada na tabela de símbolos (variáveis) */

    int           num_children;
    struct ASTNode **children;
} ASTNode;

/* ---------------------------------------------------------------------------
 * Funções de criação de nós (declaradas em ast.c)
 * ---------------------------------------------------------------------------
 * Cada função aloca e inicializa um nó específico, evitando código repetido
 * no parser. Todas recebem "line" para rastrear erros.
 * --------------------------------------------------------------------------- */

/* Cria um nó genérico com type e line; demais campos zerados/NULL */
ASTNode* create_node(NodeType type, int line);

/* Cria um nó de constante inteira, já com int_val e data_type=TYPE_INT */
ASTNode* create_int_node(int val, int line);

/* Cria um nó de constante de caractere, com char_val e data_type=TYPE_CAR */
ASTNode* create_car_node(char val, int line);

/* Cria um nó de string literal, copiando o texto com strdup */
ASTNode* create_string_node(const char *val, int line);

/* Cria um nó de referência a variável, copiando o nome com strdup */
ASTNode* create_var_node(const char *name, int line);

/* Cria um nó de operação binária, já adicionando left e right como filhos */
ASTNode* create_bin_op_node(OperatorType op, ASTNode *left, ASTNode *right, int line);

/* Cria um nó de operação unária, já adicionando expr como filho */
ASTNode* create_un_op_node(OperatorType op, ASTNode *expr, int line);

/* Adiciona "child" como filho de "parent" (redimensiona o array dinamicamente) */
void add_child(ASTNode *parent, ASTNode *child);

/* Libera recursivamente toda a memória da árvore a partir de "node" */
void free_ast(ASTNode *node);

/* Converte um DataType para string legível (para debug/mensagens de erro) */
const char* data_type_to_string(DataType type);

#endif
