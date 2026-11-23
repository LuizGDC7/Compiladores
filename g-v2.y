%code top {
    #define _DEFAULT_SOURCE
}

%code requires {
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "ast.h"
}

%{
/*
 * [G-V2] Analisador sintático, criado a partir de g-v1.y.
 * A gramática segue a Seção 2 do enunciado: seção global, seção de funções,
 * declarações com '[' ']', vetores, chamadas de função e retorne.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern int yylineno;
extern char *yytext;
extern int yylex(void);

void yyerror(const char *msg);

ASTNode *ast_root = NULL;

/*
 * [G-V2] Em "a, b[3] : int;" o tipo só é conhecido depois dos nomes.
 * Esta função aplica o tipo a todas as variáveis da lista já construída.
 */
static void apply_type(ASTNode *list, DataType base) {
    for (int i = 0; i < list->num_children; i++) {
        ASTNode *decl = list->children[i];
        decl->data_type = decl->is_vector ? vector_of(base) : base;
    }
}

/* [G-V2] Cria uma lista contendo head seguido dos itens de tail (lista à direita). */
static ASTNode* prepend_to_list(NodeType list_type, ASTNode *head, ASTNode *tail, int line) {
    ASTNode *list = create_node(list_type, line);
    add_child(list, head);
    if (tail) move_children(list, tail);
    return list;
}
%}

/* [G-V2] Habilita @n nas ações: cada nó guarda a linha exata do seu token. */
%locations

%union {
    int intval;
    char charval;
    char *str;
    ASTNode *node;
    DataType type;
}

/* [G-V2] Novos tokens: GLOBAL, FUNCAO, RETORNE e ATRIBUICAO. */
%token GLOBAL FUNCAO RETORNE ATRIBUICAO
%token PRINCIPAL INT CAR LEIA ESCREVA NOVALINHA SE ENTAO SENAO FIMSE ENQUANTO
%token OU E IGUAL DIFERENTE MAIORIGUAL MENORIGUAL
%token <str> IDENTIFICADOR CADEIACARACTERES
%token <intval> INTCONST
%token <charval> CARCONST

%type <node> Programa DeclVarGlobais VarSection ListaDeclVar ListVar
%type <node> DeclFunc ListaFuncoes ListaParametros ListaParametrosTail
%type <node> DeclPrograma Bloco ListaComando Comando
%type <node> Expr LValueExpr OrExpr AndExpr EqExpr DesigExpr AddExpr MulExpr UnExpr PrimExpr ListExpr
%type <type> Tipo

%start Programa

%%

/* [G-V2] Programa = variáveis globais + funções + principal. */
Programa
    : DeclVarGlobais DeclFunc DeclPrograma {
        $$ = create_node(NODE_PROGRAM, @3.first_line);
        add_child($$, $1);
        add_child($$, $2);
        add_child($$, $3);
        ast_root = $$;
    }
    ;

/* [G-V2] Seção global opcional; quando omitida, gera uma lista vazia. */
DeclVarGlobais
    : GLOBAL VarSection { $$ = $2; }
    | /* vazio */ { $$ = create_node(NODE_VAR_DECL_LIST, yylineno); }
    ;

/* [G-V2] A seção de variáveis agora é delimitada por '[' e ']'. */
VarSection
    : '[' ListaDeclVar ']' { $$ = $2; }
    ;

ListaDeclVar
    : ListVar ':' Tipo ';' ListaDeclVar {
        apply_type($1, $3);
        move_children($1, $5);
        $$ = $1;
    }
    | ListVar ':' Tipo ';' {
        apply_type($1, $3);
        $$ = $1;
    }
    ;

/* [G-V2] Cada nome da lista pode ser simples ou vetor com tamanho. */
ListVar
    : IDENTIFICADOR ',' ListVar {
        $$ = prepend_to_list(NODE_VAR_DECL_LIST,
                             create_var_decl_node($1, 0, 0, @1.first_line), $3, @1.first_line);
        free($1);
    }
    | IDENTIFICADOR '[' INTCONST ']' ',' ListVar {
        $$ = prepend_to_list(NODE_VAR_DECL_LIST,
                             create_var_decl_node($1, 1, $3, @1.first_line), $6, @1.first_line);
        free($1);
    }
    | IDENTIFICADOR {
        $$ = prepend_to_list(NODE_VAR_DECL_LIST,
                             create_var_decl_node($1, 0, 0, @1.first_line), NULL, @1.first_line);
        free($1);
    }
    | IDENTIFICADOR '[' INTCONST ']' {
        $$ = prepend_to_list(NODE_VAR_DECL_LIST,
                             create_var_decl_node($1, 1, $3, @1.first_line), NULL, @1.first_line);
        free($1);
    }
    ;

/* [G-V2] Seção de funções opcional: funcao [ f1(...) : t bloco  f2(...) ... ]. */
DeclFunc
    : FUNCAO '[' IDENTIFICADOR '(' ListaParametros ')' ':' Tipo Bloco ListaFuncoes ']' {
        ASTNode *func = create_func_node($3, $8, $5, $9, @3.first_line);
        $$ = prepend_to_list(NODE_FUNC_LIST, func, $10, @1.first_line);
        free($3);
    }
    | /* vazio */ { $$ = create_node(NODE_FUNC_LIST, yylineno); }
    ;

ListaFuncoes
    : IDENTIFICADOR '(' ListaParametros ')' ':' Tipo Bloco ListaFuncoes {
        ASTNode *func = create_func_node($1, $6, $3, $7, @1.first_line);
        $$ = prepend_to_list(NODE_FUNC_LIST, func, $8, @1.first_line);
        free($1);
    }
    | /* vazio */ { $$ = create_node(NODE_FUNC_LIST, yylineno); }
    ;

ListaParametros
    : ListaParametrosTail { $$ = $1; }
    | /* vazio */ { $$ = create_node(NODE_PARAM_LIST, yylineno); }
    ;

/* [G-V2] Parâmetro vetor é escrito sem tamanho: nome[] : tipo. */
ListaParametrosTail
    : IDENTIFICADOR ':' Tipo {
        $$ = prepend_to_list(NODE_PARAM_LIST,
                             create_param_node($1, 0, $3, @1.first_line), NULL, @1.first_line);
        free($1);
    }
    | IDENTIFICADOR '[' ']' ':' Tipo {
        $$ = prepend_to_list(NODE_PARAM_LIST,
                             create_param_node($1, 1, $5, @1.first_line), NULL, @1.first_line);
        free($1);
    }
    | IDENTIFICADOR ':' Tipo ',' ListaParametrosTail {
        $$ = prepend_to_list(NODE_PARAM_LIST,
                             create_param_node($1, 0, $3, @1.first_line), $5, @1.first_line);
        free($1);
    }
    | IDENTIFICADOR '[' ']' ':' Tipo ',' ListaParametrosTail {
        $$ = prepend_to_list(NODE_PARAM_LIST,
                             create_param_node($1, 1, $5, @1.first_line), $7, @1.first_line);
        free($1);
    }
    ;

/* [G-V2] O principal agora é apenas um bloco; o nó programa é criado acima. */
DeclPrograma
    : PRINCIPAL Bloco { $$ = $2; }
    ;

Bloco
    : '{' ListaComando '}' {
        $$ = create_node(NODE_BLOCK, @1.first_line);
        add_child($$, create_node(NODE_VAR_DECL_LIST, @1.first_line));
        add_child($$, $2);
    }
    | VarSection '{' ListaComando '}' {
        $$ = create_node(NODE_BLOCK, @1.first_line);
        add_child($$, $1);
        add_child($$, $3);
    }
    ;

Tipo
    : INT { $$ = TYPE_INT; }
    | CAR { $$ = TYPE_CAR; }
    ;

ListaComando
    : Comando {
        $$ = create_node(NODE_CMD_LIST, @1.first_line);
        add_child($$, $1);
    }
    | Comando ListaComando {
        $$ = prepend_to_list(NODE_CMD_LIST, $1, $2, @1.first_line);
    }
    ;

Comando
    : ';' {
        $$ = create_node(NODE_CMD_EXPR, @1.first_line);
    }
    | Expr ';' {
        $$ = create_node(NODE_CMD_EXPR, @1.first_line);
        add_child($$, $1);
    }
    | RETORNE Expr ';' { /* [G-V2] */
        $$ = create_node(NODE_CMD_RETURN, @1.first_line);
        add_child($$, $2);
    }
    | LEIA LValueExpr ';' { /* [G-V2] aceita também elemento de vetor */
        $$ = create_node(NODE_CMD_LEIA, @1.first_line);
        add_child($$, $2);
    }
    | ESCREVA Expr ';' {
        $$ = create_node(NODE_CMD_ESCREVA_EXPR, @1.first_line);
        add_child($$, $2);
    }
    | ESCREVA CADEIACARACTERES ';' {
        $$ = create_node(NODE_CMD_ESCREVA_STRING, @1.first_line);
        $$->string_val = $2;
    }
    | NOVALINHA ';' {
        $$ = create_node(NODE_CMD_NOVALINHA, @1.first_line);
    }
    | SE '(' Expr ')' ENTAO Comando FIMSE {
        $$ = create_node(NODE_CMD_IF, @1.first_line);
        add_child($$, $3);
        add_child($$, $6);
    }
    | SE '(' Expr ')' ENTAO Comando SENAO Comando FIMSE {
        $$ = create_node(NODE_CMD_IF, @1.first_line);
        add_child($$, $3);
        add_child($$, $6);
        add_child($$, $8);
    }
    | ENQUANTO '(' Expr ')' Comando {
        $$ = create_node(NODE_CMD_WHILE, @1.first_line);
        add_child($$, $3);
        add_child($$, $5);
    }
    | Bloco {
        $$ = $1;
    }
    ;

/* [G-V2] O destino da atribuição agora é um LValueExpr (variável ou v[i]). */
Expr
    : LValueExpr ATRIBUICAO Expr {
        $$ = create_node(NODE_ASSIGN, @2.first_line);
        add_child($$, $1);
        add_child($$, $3);
    }
    | OrExpr { $$ = $1; }
    ;

LValueExpr
    : IDENTIFICADOR '[' Expr ']' {
        $$ = create_array_access_node($1, $3, @1.first_line);
        free($1);
    }
    | IDENTIFICADOR {
        $$ = create_var_node($1, @1.first_line);
        free($1);
    }
    ;

OrExpr
    : OrExpr OU AndExpr {
        $$ = create_bin_op_node(OP_OU, $1, $3, @2.first_line);
    }
    | AndExpr { $$ = $1; }
    ;

AndExpr
    : AndExpr E EqExpr {
        $$ = create_bin_op_node(OP_E, $1, $3, @2.first_line);
    }
    | EqExpr { $$ = $1; }
    ;

EqExpr
    : EqExpr IGUAL DesigExpr {
        $$ = create_bin_op_node(OP_IGUAL, $1, $3, @2.first_line);
    }
    | EqExpr DIFERENTE DesigExpr {
        $$ = create_bin_op_node(OP_DIFERENTE, $1, $3, @2.first_line);
    }
    | DesigExpr { $$ = $1; }
    ;

DesigExpr
    : DesigExpr '<' AddExpr {
        $$ = create_bin_op_node(OP_LESS, $1, $3, @2.first_line);
    }
    | DesigExpr '>' AddExpr {
        $$ = create_bin_op_node(OP_GREATER, $1, $3, @2.first_line);
    }
    | DesigExpr MAIORIGUAL AddExpr {
        $$ = create_bin_op_node(OP_GE, $1, $3, @2.first_line);
    }
    | DesigExpr MENORIGUAL AddExpr {
        $$ = create_bin_op_node(OP_LE, $1, $3, @2.first_line);
    }
    | AddExpr { $$ = $1; }
    ;

AddExpr
    : AddExpr '+' MulExpr {
        $$ = create_bin_op_node(OP_ADD, $1, $3, @2.first_line);
    }
    | AddExpr '-' MulExpr {
        $$ = create_bin_op_node(OP_SUB, $1, $3, @2.first_line);
    }
    | MulExpr { $$ = $1; }
    ;

MulExpr
    : MulExpr '*' UnExpr {
        $$ = create_bin_op_node(OP_MUL, $1, $3, @2.first_line);
    }
    | MulExpr '/' UnExpr {
        $$ = create_bin_op_node(OP_DIV, $1, $3, @2.first_line);
    }
    | UnExpr { $$ = $1; }
    ;

UnExpr
    : '-' PrimExpr {
        $$ = create_un_op_node(OP_NEG, $2, @1.first_line);
    }
    | '!' PrimExpr {
        $$ = create_un_op_node(OP_NOT, $2, @1.first_line);
    }
    | PrimExpr { $$ = $1; }
    ;

/* [G-V2] Novas formas primárias: chamada de função e acesso a vetor. */
PrimExpr
    : IDENTIFICADOR '(' ListExpr ')' {
        $$ = create_call_node($1, $3, @1.first_line);
        free($1);
    }
    | IDENTIFICADOR '(' ')' {
        $$ = create_call_node($1, NULL, @1.first_line);
        free($1);
    }
    | IDENTIFICADOR '[' Expr ']' {
        $$ = create_array_access_node($1, $3, @1.first_line);
        free($1);
    }
    | IDENTIFICADOR {
        $$ = create_var_node($1, @1.first_line);
        free($1);
    }
    | CARCONST {
        $$ = create_car_node($1, @1.first_line);
    }
    | INTCONST {
        $$ = create_int_node($1, @1.first_line);
    }
    | '(' Expr ')' {
        $$ = $2;
    }
    ;

/* [G-V2] Argumentos de uma chamada, na ordem em que aparecem. */
ListExpr
    : Expr {
        $$ = create_node(NODE_EXPR_LIST, @1.first_line);
        add_child($$, $1);
    }
    | ListExpr ',' Expr {
        add_child($1, $3);
        $$ = $1;
    }
    ;

%%

void yyerror(const char *msg) {
    (void)msg;
    printf("ERRO: SINTATICO NA LINHA %d\n", yylineno);
    exit(1);
}
