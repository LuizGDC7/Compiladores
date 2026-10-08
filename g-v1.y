%code top {
    #define _DEFAULT_SOURCE
}

%code requires {
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "ast.h"

    typedef struct StringList {
        char *name;
        struct StringList *next;
    } StringList;
}

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern int yylineno;
extern char *yytext;
extern int yylex(void);

void yyerror(const char *msg);

ASTNode *ast_root = NULL;
%}

%code {
static StringList* create_string_list(char *name) {
    StringList *l = (StringList*) malloc(sizeof(StringList));
    l->name = strdup(name);
    l->next = NULL;
    return l;
}

static StringList* append_string_list(StringList *head, char *name) {
    StringList *node = create_string_list(name);
    if (!head) return node;
    StringList *curr = head;
    while (curr->next) curr = curr->next;
    curr->next = node;
    return head;
}
}

%union {
    int intval;
    char charval;
    char *str;
    ASTNode *node;
    DataType type;
    StringList *str_list;
}

%token PRINCIPAL INT CAR LEIA ESCREVA NOVALINHA SE ENTAO SENAO FIMSE ENQUANTO
%token OU E IGUAL DIFERENTE MAIORIGUAL MENORIGUAL
%token <str> IDENTIFICADOR CADEIACARACTERES
%token <intval> INTCONST
%token <charval> CARCONST

%type <node> Programa DeclPrograma Bloco VarSection ListaDeclVar ListaComando Comando
%type <node> Expr OrExpr AndExpr EqExpr DesigExpr AddExpr MulExpr UnExpr PrimExpr
%type <type> Tipo
%type <str_list> DeclVar

%start Programa

%%

Programa
    : DeclPrograma {
        ast_root = $1;
    }
    ;

DeclPrograma
    : PRINCIPAL Bloco {
        $$ = create_node(NODE_PROGRAM, yylineno);
        add_child($$, $2);
    }
    ;

Bloco
    : '{' ListaComando '}' {
        $$ = create_node(NODE_BLOCK, yylineno);
        ASTNode *empty_decls = create_node(NODE_VAR_DECL_LIST, yylineno);
        add_child($$, empty_decls);
        add_child($$, $2);
    }
    | VarSection '{' ListaComando '}' {
        $$ = create_node(NODE_BLOCK, yylineno);
        add_child($$, $1);
        add_child($$, $3);
    }
    ;

VarSection
    : '{' ListaDeclVar '}' {
        $$ = $2;
    }
    ;

ListaDeclVar
    : IDENTIFICADOR DeclVar ':' Tipo ';' ListaDeclVar {
        ASTNode *d1 = create_node(NODE_VAR_DECL, yylineno);
        d1->string_val = strdup($1);
        d1->data_type = $4;
        
        ASTNode *new_list = create_node(NODE_VAR_DECL_LIST, yylineno);
        add_child(new_list, d1);
        
        StringList *curr = $2;
        while (curr) {
            ASTNode *d = create_node(NODE_VAR_DECL, yylineno);
            d->string_val = strdup(curr->name);
            d->data_type = $4;
            add_child(new_list, d);
            curr = curr->next;
        }
        for (int i = 0; i < $6->num_children; i++) {
            add_child(new_list, $6->children[i]);
        }
        $$ = new_list;
    }
    | IDENTIFICADOR DeclVar ':' Tipo ';' {
        $$ = create_node(NODE_VAR_DECL_LIST, yylineno);
        ASTNode *d1 = create_node(NODE_VAR_DECL, yylineno);
        d1->string_val = strdup($1);
        d1->data_type = $4;
        add_child($$, d1);

        StringList *curr = $2;
        while (curr) {
            ASTNode *d = create_node(NODE_VAR_DECL, yylineno);
            d->string_val = strdup(curr->name);
            d->data_type = $4;
            add_child($$, d);
            curr = curr->next;
        }
    }
    ;

DeclVar
    : /* vazio */ {
        $$ = NULL;
    }
    | ',' IDENTIFICADOR DeclVar {
        $$ = append_string_list(NULL, $2);
        StringList *curr = $3;
        while (curr) {
            append_string_list($$, curr->name);
            curr = curr->next;
        }
    }
    ;

Tipo
    : INT { $$ = TYPE_INT; }
    | CAR { $$ = TYPE_CAR; }
    ;

ListaComando
    : Comando {
        $$ = create_node(NODE_CMD_LIST, yylineno);
        add_child($$, $1);
    }
    | Comando ListaComando {
        $$ = create_node(NODE_CMD_LIST, yylineno);
        add_child($$, $1);
        for (int i = 0; i < $2->num_children; i++) {
            add_child($$, $2->children[i]);
        }
    }
    ;

Comando
    : ';' {
        $$ = create_node(NODE_CMD_EXPR, yylineno);
    }
    | Expr ';' {
        $$ = create_node(NODE_CMD_EXPR, yylineno);
        add_child($$, $1);
    }
    | LEIA IDENTIFICADOR ';' {
        $$ = create_node(NODE_CMD_LEIA, yylineno);
        $$->string_val = strdup($2);
    }
    | ESCREVA Expr ';' {
        $$ = create_node(NODE_CMD_ESCREVA_EXPR, yylineno);
        add_child($$, $2);
    }
    | ESCREVA CADEIACARACTERES ';' {
        $$ = create_node(NODE_CMD_ESCREVA_STRING, yylineno);
        $$->string_val = strdup($2);
    }
    | NOVALINHA ';' {
        $$ = create_node(NODE_CMD_NOVALINHA, yylineno);
    }
    | SE '(' Expr ')' ENTAO Comando FIMSE {
        $$ = create_node(NODE_CMD_IF, yylineno);
        add_child($$, $3);
        add_child($$, $6);
    }
    | SE '(' Expr ')' ENTAO Comando SENAO Comando FIMSE {
        $$ = create_node(NODE_CMD_IF, yylineno);
        add_child($$, $3);
        add_child($$, $6);
        add_child($$, $8);
    }
    | ENQUANTO '(' Expr ')' Comando {
        $$ = create_node(NODE_CMD_WHILE, yylineno);
        add_child($$, $3);
        add_child($$, $5);
    }
    | Bloco {
        $$ = $1;
    }
    ;

Expr
    : OrExpr { $$ = $1; }
    | IDENTIFICADOR '=' Expr {
        $$ = create_node(NODE_ASSIGN, yylineno);
        add_child($$, create_var_node($1, yylineno));
        add_child($$, $3);
    }
    ;

OrExpr
    : OrExpr OU AndExpr {
        $$ = create_bin_op_node(OP_OU, $1, $3, yylineno);
    }
    | AndExpr { $$ = $1; }
    ;

AndExpr
    : AndExpr E EqExpr {
        $$ = create_bin_op_node(OP_E, $1, $3, yylineno);
    }
    | EqExpr { $$ = $1; }
    ;

EqExpr
    : EqExpr IGUAL DesigExpr {
        $$ = create_bin_op_node(OP_IGUAL, $1, $3, yylineno);
    }
    | EqExpr DIFERENTE DesigExpr {
        $$ = create_bin_op_node(OP_DIFERENTE, $1, $3, yylineno);
    }
    | DesigExpr { $$ = $1; }
    ;

DesigExpr
    : DesigExpr '<' AddExpr {
        $$ = create_bin_op_node(OP_LESS, $1, $3, yylineno);
    }
    | DesigExpr '>' AddExpr {
        $$ = create_bin_op_node(OP_GREATER, $1, $3, yylineno);
    }
    | DesigExpr MAIORIGUAL AddExpr {
        $$ = create_bin_op_node(OP_GE, $1, $3, yylineno);
    }
    | DesigExpr MENORIGUAL AddExpr {
        $$ = create_bin_op_node(OP_LE, $1, $3, yylineno);
    }
    | AddExpr { $$ = $1; }
    ;

AddExpr
    : AddExpr '+' MulExpr {
        $$ = create_bin_op_node(OP_ADD, $1, $3, yylineno);
    }
    | AddExpr '-' MulExpr {
        $$ = create_bin_op_node(OP_SUB, $1, $3, yylineno);
    }
    | MulExpr { $$ = $1; }
    ;

MulExpr
    : MulExpr '*' UnExpr {
        $$ = create_bin_op_node(OP_MUL, $1, $3, yylineno);
    }
    | MulExpr '/' UnExpr {
        $$ = create_bin_op_node(OP_DIV, $1, $3, yylineno);
    }
    | UnExpr { $$ = $1; }
    ;

UnExpr
    : '-' PrimExpr {
        $$ = create_un_op_node(OP_NEG, $2, yylineno);
    }
    | '!' PrimExpr {
        $$ = create_un_op_node(OP_NOT, $2, yylineno);
    }
    | PrimExpr { $$ = $1; }
    ;

PrimExpr
    : IDENTIFICADOR {
        $$ = create_var_node($1, yylineno);
    }
    | CARCONST {
        $$ = create_car_node($1, yylineno);
    }
    | INTCONST {
        $$ = create_int_node($1, yylineno);
    }
    | '(' Expr ')' {
        $$ = $2;
    }
    ;

%%

void yyerror(const char *msg) {
    (void)msg;
    printf("ERRO: SINTATICO NA LINHA %d\n", yylineno);
    exit(1);
}
