#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TYPE_NONE,
    TYPE_INT,
    TYPE_CAR,
    TYPE_UNKNOWN
} DataType;

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

void add_child(ASTNode *parent, ASTNode *child);
void free_ast(ASTNode *node);
const char* data_type_to_string(DataType type);

#endif
