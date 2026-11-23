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
    node->is_vector = 0;   /* [G-V2] por padrão o nó não representa um vetor */
    node->array_size = 0;  /* [G-V2] */
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

/* [G-V2] Acesso a elemento: guarda o nome do vetor e a expressão do índice. */
ASTNode* create_array_access_node(const char *name, ASTNode *index, int line) {
    ASTNode *node = create_node(NODE_ARRAY_ACCESS, line);
    node->string_val = strdup(name ? name : "");
    add_child(node, index);
    return node;
}

/*
 * [G-V2] Chamada de função. Os argumentos chegam em uma lista auxiliar
 * (NODE_EXPR_LIST), que é absorvida: cada argumento vira filho direto da chamada.
 * args == NULL representa uma chamada sem argumentos.
 */
ASTNode* create_call_node(const char *name, ASTNode *args, int line) {
    ASTNode *node = create_node(NODE_CALL, line);
    node->string_val = strdup(name ? name : "");
    if (args) move_children(node, args);
    return node;
}

/*
 * [G-V2] Declaração de variável. O tipo ainda não é conhecido neste ponto,
 * pois na gramática ele aparece depois da lista de nomes ("a, b[3] : int;").
 * A ação do Bison preenche data_type quando reduz a declaração completa.
 */
ASTNode* create_var_decl_node(const char *name, int is_vector, int size, int line) {
    ASTNode *node = create_node(NODE_VAR_DECL, line);
    node->string_val = strdup(name ? name : "");
    node->is_vector = is_vector;
    node->array_size = size;
    return node;
}

/* [G-V2] Parâmetro formal: "x : int" ou "x[] : int" (vetor sem tamanho). */
ASTNode* create_param_node(const char *name, int is_vector, DataType base, int line) {
    ASTNode *node = create_node(NODE_PARAM, line);
    node->string_val = strdup(name ? name : "");
    node->is_vector = is_vector;
    node->data_type = is_vector ? vector_of(base) : base;
    return node;
}

/* [G-V2] Função: nome, tipo de retorno, lista de parâmetros e bloco. */
ASTNode* create_func_node(const char *name, DataType ret, ASTNode *params, ASTNode *body, int line) {
    ASTNode *node = create_node(NODE_FUNC_DECL, line);
    node->string_val = strdup(name ? name : "");
    node->data_type = ret;
    add_child(node, params);
    add_child(node, body);
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

/*
 * [G-V2] Usada pelas listas da gramática (variáveis, parâmetros, funções...).
 * Transfere os filhos e libera apenas o nó "casca" de src, evitando que o mesmo
 * filho fique em duas listas e seja liberado duas vezes por free_ast.
 */
void move_children(ASTNode *dst, ASTNode *src) {
    if (!dst || !src) return;
    for (int i = 0; i < src->num_children; i++) {
        add_child(dst, src->children[i]);
    }
    free(src->children);
    free(src->string_val);
    free(src);
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
        case TYPE_INT_VET: return "vetor de int"; /* [G-V2] */
        case TYPE_CAR_VET: return "vetor de car"; /* [G-V2] */
        case TYPE_NONE: return "none";
        default: return "unknown";
    }
}

/* [G-V2] Indica se o tipo é um dos tipos vetor. */
int is_vector_type(DataType type) {
    return type == TYPE_INT_VET || type == TYPE_CAR_VET;
}

/* [G-V2] Converte um tipo básico no tipo vetor correspondente. */
DataType vector_of(DataType base) {
    return base == TYPE_CAR ? TYPE_CAR_VET : TYPE_INT_VET;
}
