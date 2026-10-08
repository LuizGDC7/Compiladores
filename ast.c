/*
 * =============================================================================
 * ast.c — Implementação da Árvore Sintática Abstrata (AST)
 * =============================================================================
 *
 * RESPONSABILIDADE:
 *   Implementa as funções que criam, manipulam e liberam os nós da AST.
 *   Este arquivo NÃO contém lógica de compilação — apenas gerenciamento
 *   de memória e inicialização de nós.
 *
 * RELAÇÃO COM OUTROS ARQUIVOS:
 *   → g-v1.y  usa as funções daqui para montar a árvore durante o parsing
 *   → semantic.c  lê a árvore para verificar tipos e escopos
 *   → codegen.c   lê a árvore para gerar código Assembly MIPS
 *   → main.c      chama free_ast() no final para liberar toda a memória
 * =============================================================================
 */

#define _DEFAULT_SOURCE
#include "ast.h"

/*
 * create_node — Cria um nó genérico da AST
 * -----------------------------------------------------------------------
 * Aloca memória para um novo ASTNode e inicializa TODOS os campos com
 * valores neutros (zero, NULL, TYPE_NONE). Isso evita leitura de lixo
 * de memória nas fases posteriores.
 *
 * Parâmetros:
 *   type → categoria do nó (ex: NODE_ASSIGN, NODE_BIN_OP, etc.)
 *   line → linha do código-fonte de onde esse nó se originou
 *
 * Retorna: ponteiro para o novo nó, ou encerra o programa se falhar
 *
 * Exemplo de uso (no parser g-v1.y):
 *   ASTNode *bloco = create_node(NODE_BLOCK, yylineno);
 */
ASTNode* create_node(NodeType type, int line) {
    ASTNode *node = (ASTNode*) malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Erro de alocação de memória para nó AST\n");
        exit(1);
    }
    node->type        = type;
    node->line        = line;
    node->data_type   = TYPE_NONE;   /* será preenchido pela análise semântica */
    node->string_val  = NULL;
    node->int_val     = 0;
    node->char_val    = 0;
    node->symbol      = NULL;        /* associado na análise semântica, se variável */
    node->num_children = 0;
    node->children    = NULL;        /* array dinâmico, cresce com add_child() */
    return node;
}

/*
 * create_int_node — Cria um nó de constante inteira
 * -----------------------------------------------------------------------
 * Atalho para criar nós do tipo NODE_INT_CONST, já armazenando o valor
 * numérico e marcando o tipo como TYPE_INT.
 *
 * Exemplo: o literal "42" no código-fonte vira:
 *   ASTNode { type=NODE_INT_CONST, int_val=42, data_type=TYPE_INT }
 */
ASTNode* create_int_node(int val, int line) {
    ASTNode *node = create_node(NODE_INT_CONST, line);
    node->int_val   = val;
    node->data_type = TYPE_INT;
    return node;
}

/*
 * create_car_node — Cria um nó de constante de caractere
 * -----------------------------------------------------------------------
 * Atalho para criar nós do tipo NODE_CAR_CONST, armazenando o valor char
 * e marcando o tipo como TYPE_CAR.
 *
 * Exemplo: o literal 'A' no código-fonte vira:
 *   ASTNode { type=NODE_CAR_CONST, char_val='A', data_type=TYPE_CAR }
 */
ASTNode* create_car_node(char val, int line) {
    ASTNode *node = create_node(NODE_CAR_CONST, line);
    node->char_val  = val;
    node->data_type = TYPE_CAR;
    return node;
}

/*
 * create_string_node — Cria um nó de string literal
 * -----------------------------------------------------------------------
 * Usado para strings entre aspas como "Resultado: ". O texto é copiado
 * com strdup() porque o buffer do lexer pode ser reutilizado.
 *
 * Exemplo: o literal "ola mundo" vira:
 *   ASTNode { type=NODE_STRING_CONST, string_val="\"ola mundo\"" }
 */
ASTNode* create_string_node(const char *val, int line) {
    ASTNode *node = create_node(NODE_STRING_CONST, line);
    node->string_val = strdup(val ? val : "");
    return node;
}

/*
 * create_var_node — Cria um nó de referência a variável
 * -----------------------------------------------------------------------
 * Representa o USO de uma variável já declarada (ex: "x" em "escreva x;").
 * O nome é copiado com strdup(). O data_type será preenchido durante a
 * análise semântica ao consultar a tabela de símbolos.
 *
 * Exemplo: a referência a "contador" vira:
 *   ASTNode { type=NODE_VAR, string_val="contador" }
 */
ASTNode* create_var_node(const char *name, int line) {
    ASTNode *node = create_node(NODE_VAR, line);
    node->string_val = strdup(name ? name : "");
    return node;
}

/*
 * create_bin_op_node — Cria um nó de operação binária
 * -----------------------------------------------------------------------
 * Representa operações com dois operandos: aritméticas (+, -, *, /),
 * relacionais (<, >, >=, <=, ==, !=) e lógicas (||, &).
 *
 * O operador é armazenado em int_val (como o enum OperatorType).
 * Os filhos são: [0] = operando esquerdo, [1] = operando direito.
 *
 * Exemplo: "a + b * 2" no código-fonte gera:
 *   NODE_BIN_OP(OP_ADD)
 *       ├── NODE_VAR("a")
 *       └── NODE_BIN_OP(OP_MUL)
 *               ├── NODE_VAR("b")
 *               └── NODE_INT_CONST(2)
 */
ASTNode* create_bin_op_node(OperatorType op, ASTNode *left, ASTNode *right, int line) {
    ASTNode *node = create_node(NODE_BIN_OP, line);
    node->int_val = op;    /* armazena o operador como inteiro (cast de enum) */
    add_child(node, left);
    add_child(node, right);
    return node;
}

/*
 * create_un_op_node — Cria um nó de operação unária
 * -----------------------------------------------------------------------
 * Representa operações com um único operando:
 *   OP_NEG → negação aritmética: "-x"
 *   OP_NOT → negação lógica: "!x"
 *
 * O operador fica em int_val; o único filho é a expressão operada.
 *
 * Exemplo: "-resultado" no código-fonte gera:
 *   NODE_UN_OP(OP_NEG)
 *       └── NODE_VAR("resultado")
 */
ASTNode* create_un_op_node(OperatorType op, ASTNode *expr, int line) {
    ASTNode *node = create_node(NODE_UN_OP, line);
    node->int_val = op;
    add_child(node, expr);
    return node;
}

/*
 * add_child — Adiciona um nó filho a um nó pai
 * -----------------------------------------------------------------------
 * Redimensiona dinamicamente o array "children" do pai com realloc() e
 * insere o filho no final. Isso permite que cada nó tenha número variável
 * de filhos (ex: NODE_CMD_IF pode ter 2 ou 3 filhos).
 *
 * Parâmetros:
 *   parent → nó que receberá o filho
 *   child  → nó filho a ser adicionado
 *
 * IMPORTANTE: se parent ou child for NULL, a função simplesmente retorna.
 * Isso evita crashes quando o parser cria nós opcionais.
 */
void add_child(ASTNode *parent, ASTNode *child) {
    if (!parent || !child) return;
    parent->num_children++;
    parent->children = (ASTNode**) realloc(
        parent->children,
        parent->num_children * sizeof(ASTNode*)
    );
    if (!parent->children) {
        fprintf(stderr, "Erro ao realocar memória para filhos do nó AST\n");
        exit(1);
    }
    parent->children[parent->num_children - 1] = child;
}

/*
 * free_ast — Libera recursivamente toda a memória da árvore
 * -----------------------------------------------------------------------
 * Percorre a árvore em pós-ordem (filhos antes do pai) para garantir que
 * nenhum ponteiro filho seja acessado após ser liberado.
 *
 * Libera:
 *   1. Recursivamente cada filho
 *   2. O array "children"
 *   3. O string_val (se houver)
 *   4. O próprio nó
 *
 * Chamado em main.c após a geração de código para evitar memory leaks.
 */
void free_ast(ASTNode *node) {
    if (!node) return;
    /* percorre e libera cada filho antes de liberar o pai */
    for (int i = 0; i < node->num_children; i++) {
        free_ast(node->children[i]);
    }
    if (node->children)  free(node->children);
    if (node->string_val) free(node->string_val);
    free(node);
}

/*
 * data_type_to_string — Converte DataType para string legível
 * -----------------------------------------------------------------------
 * Utilitário auxiliar. Retorna o nome textual do tipo para ser usado em
 * mensagens de erro ou de debug.
 *
 * Retorna: "int", "car", "none", ou "unknown"
 */
const char* data_type_to_string(DataType type) {
    switch (type) {
        case TYPE_INT:  return "int";
        case TYPE_CAR:  return "car";
        case TYPE_NONE: return "none";
        default:        return "unknown";
    }
}
