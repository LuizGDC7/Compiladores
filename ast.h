#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * [G-V2] Tipos de dados.
 * Além de int e car, a linguagem agora possui "vetor de int" e "vetor de car".
 * Cada vetor é um tipo próprio: assim, comparar dois tipos com == / != já
 * detecta tanto "int x car" quanto "int x vetor de int", que são proibidos.
 */
typedef enum {
    TYPE_NONE,
    TYPE_INT,
    TYPE_CAR,
    TYPE_INT_VET,   /* [G-V2] vetor de int */
    TYPE_CAR_VET,   /* [G-V2] vetor de car */
    TYPE_UNKNOWN
} DataType;

typedef enum {
    NODE_PROGRAM,       /* [G-V2] filhos: [globais, funções, bloco principal] */
    NODE_BLOCK,
    NODE_VAR_DECL_LIST,
    NODE_VAR_DECL,
    NODE_FUNC_LIST,     /* [G-V2] lista de declarações de funções */
    NODE_FUNC_DECL,     /* [G-V2] nome + tipo; filhos: [parâmetros, bloco] */
    NODE_PARAM_LIST,    /* [G-V2] lista de parâmetros formais */
    NODE_PARAM,         /* [G-V2] um parâmetro formal (simples ou vetor) */
    NODE_CMD_LIST,
    NODE_CMD_EXPR,
    NODE_CMD_RETURN,    /* [G-V2] comando retorne; filho: expressão */
    NODE_CMD_LEIA,      /* [G-V2] agora tem como filho um l-value */
    NODE_CMD_ESCREVA_EXPR,
    NODE_CMD_ESCREVA_STRING,
    NODE_CMD_NOVALINHA,
    NODE_CMD_IF,
    NODE_CMD_WHILE,
    NODE_ASSIGN,        /* [G-V2] filho 0 pode ser NODE_VAR ou NODE_ARRAY_ACCESS */
    NODE_BIN_OP,
    NODE_UN_OP,
    NODE_VAR,
    NODE_ARRAY_ACCESS,  /* [G-V2] identificador de vetor; filho: expressão índice */
    NODE_CALL,          /* [G-V2] chamada de função; filhos: argumentos */
    NODE_EXPR_LIST,     /* [G-V2] lista auxiliar de argumentos de chamada */
    NODE_INT_CONST,
    NODE_CAR_CONST,
    NODE_STRING_CONST
} NodeType;

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

struct Symbol;

typedef struct ASTNode {
    NodeType type;
    int line;
    DataType data_type;

    char *string_val;   /* For IDENTIFICADOR, string literals */
    int int_val;        /* For INTCONST, OperatorType */
    char char_val;      /* For CARCONST */

    /* [G-V2] Usados por NODE_VAR_DECL e NODE_PARAM. */
    int is_vector;      /* 1 se foi declarado com colchetes */
    int array_size;     /* quantidade de elementos (somente em NODE_VAR_DECL) */

    struct Symbol *symbol; /* Associated symbol table entry (for variables) */

    int num_children;
    struct ASTNode **children;
} ASTNode;

ASTNode* create_node(NodeType type, int line);
ASTNode* create_int_node(int val, int line);
ASTNode* create_car_node(char val, int line);
ASTNode* create_string_node(const char *val, int line);
ASTNode* create_var_node(const char *name, int line);
ASTNode* create_bin_op_node(OperatorType op, ASTNode *left, ASTNode *right, int line);
ASTNode* create_un_op_node(OperatorType op, ASTNode *expr, int line);

/* [G-V2] Construtores dos novos nós. */
ASTNode* create_array_access_node(const char *name, ASTNode *index, int line);
ASTNode* create_call_node(const char *name, ASTNode *args, int line);
ASTNode* create_var_decl_node(const char *name, int is_vector, int size, int line);
ASTNode* create_param_node(const char *name, int is_vector, DataType base, int line);
ASTNode* create_func_node(const char *name, DataType ret, ASTNode *params, ASTNode *body, int line);

void add_child(ASTNode *parent, ASTNode *child);
/* [G-V2] Move os filhos de src para o fim de dst e libera o nó src. */
void move_children(ASTNode *dst, ASTNode *src);
void free_ast(ASTNode *node);
const char* data_type_to_string(DataType type);

/* [G-V2] Funções auxiliares para trabalhar com os tipos vetor. */
int is_vector_type(DataType type);
DataType vector_of(DataType base);

#endif
