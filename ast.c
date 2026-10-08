#define _DEFAULT_SOURCE
#include "ast.h"

ASTNode* create_node(NodeType type, int line) {
    ASTNode *node = (ASTNode*) malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Erro de alocação de memória para nó AST\n");
        exit(1);
    }
    node->type = type;
    node->line = line;
    node->data_type = TYPE_NONE;
    node->string_val = NULL;
    node->int_val = 0;
    node->char_val = 0;
    node->symbol = NULL;
    node->num_children = 0;
    node->children = NULL;
    return node;
}

ASTNode* create_int_node(int val, int line) {
    ASTNode *node = create_node(NODE_INT_CONST, line);
    node->int_val = val;
    node->data_type = TYPE_INT;
    return node;
}

ASTNode* create_car_node(char val, int line) {
    ASTNode *node = create_node(NODE_CAR_CONST, line);
    node->char_val = val;
    node->data_type = TYPE_CAR;
    return node;
}

ASTNode* create_string_node(const char *val, int line) {
    ASTNode *node = create_node(NODE_STRING_CONST, line);
    node->string_val = strdup(val ? val : "");
    return node;
}

ASTNode* create_var_node(const char *name, int line) {
    ASTNode *node = create_node(NODE_VAR, line);
    node->string_val = strdup(name ? name : "");
    return node;
}

ASTNode* create_bin_op_node(OperatorType op, ASTNode *left, ASTNode *right, int line) {
    ASTNode *node = create_node(NODE_BIN_OP, line);
    node->int_val = op;
    add_child(node, left);
    add_child(node, right);
    return node;
}

ASTNode* create_un_op_node(OperatorType op, ASTNode *expr, int line) {
    ASTNode *node = create_node(NODE_UN_OP, line);
    node->int_val = op;
    add_child(node, expr);
    return node;
}

void add_child(ASTNode *parent, ASTNode *child) {
    if (!parent || !child) return;
    parent->num_children++;
    parent->children = (ASTNode**) realloc(parent->children, parent->num_children * sizeof(ASTNode*));
    if (!parent->children) {
        fprintf(stderr, "Erro ao realocar memória para filhos do nó AST\n");
        exit(1);
    }
    parent->children[parent->num_children - 1] = child;
}

void free_ast(ASTNode *node) {
    if (!node) return;
    for (int i = 0; i < node->num_children; i++) {
        free_ast(node->children[i]);
    }
    if (node->children) free(node->children);
    if (node->string_val) free(node->string_val);
    free(node);
}

const char* data_type_to_string(DataType type) {
    switch (type) {
        case TYPE_INT: return "int";
        case TYPE_CAR: return "car";
        case TYPE_NONE: return "none";
        default: return "unknown";
    }
}
