/*
 * =============================================================================
 * g-v1.y — Analisador Sintático (Bison)
 * =============================================================================
 *
 * RESPONSABILIDADE:
 *   Validar se as palavras (tokens gerados pelo g-v1.l) formam frases
 *   válidas dentro das "regras gramaticais" da linguagem G-V1.
 *   Ao passo em que reconhece essas regras (Bottom-Up Parser), já
 *   constrói em paralelo a Árvore Sintática Abstrata (AST).
 *
 * ESTRUTURA YACC/BISON:
 *   %code e %{ ... %} → Código em linguagem C puro copiado pro arquivo gerado.
 *   %union            → Tipos que as variáveis sintáticas $$ ou $1 podem assumir.
 *   %token            → Tokens terminais (folhas) lidos do lexer.
 *   %type             → Regras não-terminais (nós/galhos) que formam a árvore.
 *   %% regras %%      → Regras Gramaticais em formato BNF.
 * =============================================================================
 */

%code top {
    #define _DEFAULT_SOURCE
}

%code requires {
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "ast.h"

    /* Estrutura para ajudar na declaração encadeada de variáveis "a, b, c: int" */
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

/* Variáveis expostas pelo Flex */
extern int yylineno;
extern char *yytext;
extern int yylex(void);

void yyerror(const char *msg);

/* Raiz da AST gerada; é acessada externamente no main.c */
ASTNode *ast_root = NULL;
%}

%code {
/* Funções auxiliares para montar a lista de nomes da regra "DeclVar" */
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

/*
 * O union define a variável yylval compartilhada com o lexer.
 * Os tipos de valores carregados nos nós durante o parse transitam aqui.
 */
%union {
    int intval;         /* Valores inteiros ex: regras INTCONST */
    char charval;       /* Valores de caractere ex: CARCONST */
    char *str;          /* Textos, lexemas */
    ASTNode *node;      /* O nó resultante (sub-árvore) que uma regra devolve */
    DataType type;      /* Tipo INT/CAR, etc */
    StringList *str_list;/* Lista encadeada de nomes na mesma declaração */
}

/* TOKENS - Produzidos pelo lexer */
%token PRINCIPAL INT CAR LEIA ESCREVA NOVALINHA SE ENTAO SENAO FIMSE ENQUANTO
%token OU E IGUAL DIFERENTE MAIORIGUAL MENORIGUAL
%token <str> IDENTIFICADOR CADEIACARACTERES
%token <intval> INTCONST
%token <charval> CARCONST

/* TYPES - Define o retorno da união acima para regras não terminais específicas */
%type <node> Programa DeclPrograma Bloco VarSection ListaDeclVar ListaComando Comando
%type <node> Expr OrExpr AndExpr EqExpr DesigExpr AddExpr MulExpr UnExpr PrimExpr
%type <type> Tipo
%type <str_list> DeclVar

%start Programa /* A regra raiz de onde tudo deve ser originado */

%%

/* --------------------------------------------------------------------------
 * Regras de Gramática (BNF com ações em C)
 * -------------------------------------------------------------------------- */

Programa
    : DeclPrograma {
        /* A regra inicial finaliza e guarda a árvore na raiz 'ast_root' */
        ast_root = $1;
    }
    ;

DeclPrograma
    : PRINCIPAL Bloco {
        /* Regra: Palavra-chave principal, e um Bloco em seguida */
        $$ = create_node(NODE_PROGRAM, yylineno);
        add_child($$, $2); /* Adiciona a AST do bloco como filho do Programa */
    }
    ;

Bloco
    /* Bloco apenas de comandos: {...} */
    : '{' ListaComando '}' {
        $$ = create_node(NODE_BLOCK, yylineno);
        ASTNode *empty_decls = create_node(NODE_VAR_DECL_LIST, yylineno);
        add_child($$, empty_decls); /* Coloca uma lista de declarações de var. vazia */
        add_child($$, $2);          /* Lista de comandos */
    }
    /* Bloco com declaração de variáveis E de comandos: { vars } { comandos } */
    | VarSection '{' ListaComando '}' {
        $$ = create_node(NODE_BLOCK, yylineno);
        add_child($$, $1); /* Regra VarSection (declarações) */
        add_child($$, $3); /* Lista de comandos */
    }
    ;

VarSection
    /* Bloco VarSection (seção de variáveis) no formato { ... } */
    : '{' ListaDeclVar '}' {
        $$ = $2; /* Repassa o nó retornado pela regra ListaDeclVar */
    }
    ;

ListaDeclVar
    /* Lista de declarações recursiva (pode ter várias linhas) */
    : IDENTIFICADOR DeclVar ':' Tipo ';' ListaDeclVar {
        /* Cria a declaração para a primeira variável da linha (ex: x) */
        ASTNode *d1 = create_node(NODE_VAR_DECL, yylineno);
        d1->string_val = strdup($1);
        d1->data_type = $4;
        
        /* O nó pai que lista as declarações. */
        ASTNode *new_list = create_node(NODE_VAR_DECL_LIST, yylineno);
        add_child(new_list, d1);
        
        /* Se houver vars adicionais na mesma linha, ex: "x, y, z" (em $2) */
        StringList *curr = $2;
        while (curr) {
            ASTNode *d = create_node(NODE_VAR_DECL, yylineno);
            d->string_val = strdup(curr->name);
            d->data_type = $4;
            add_child(new_list, d);
            curr = curr->next;
        }
        /* Pega os filhos da regra ListaDeclVar (chamada recursiva) ($6) e anexa nesta lista nova */
        for (int i = 0; i < $6->num_children; i++) {
            add_child(new_list, $6->children[i]);
        }
        $$ = new_list;
    }
    /* Caso base (última declaração) */
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
    /* Regra para agrupar várias variáveis na mesma linha (vazio ou , ident) */
    : /* vazio */ {
        $$ = NULL;
    }
    | ',' IDENTIFICADOR DeclVar {
        /* Encadeará Strings retornando o Head em $$ */
        $$ = append_string_list(NULL, $2);
        StringList *curr = $3;
        while (curr) {
            append_string_list($$, curr->name);
            curr = curr->next;
        }
    }
    ;

Tipo
    /* Tipos em G-V1 */
    : INT { $$ = TYPE_INT; }
    | CAR { $$ = TYPE_CAR; }
    ;

ListaComando
    /* Um comando, ou múltiplos comandos separados recursivamente */
    : Comando {
        $$ = create_node(NODE_CMD_LIST, yylineno);
        add_child($$, $1);
    }
    | Comando ListaComando {
        $$ = create_node(NODE_CMD_LIST, yylineno);
        add_child($$, $1);
        /* Copia os comandos da regra ListaComando subsequente */
        for (int i = 0; i < $2->num_children; i++) {
            add_child($$, $2->children[i]);
        }
    }
    ;

Comando
    /* Vários tipos de comandos suportados... */
    : ';' { /* Comando nulo (vazio) */
        $$ = create_node(NODE_CMD_EXPR, yylineno);
    }
    | Expr ';' { /* Comando de Expressão */
        $$ = create_node(NODE_CMD_EXPR, yylineno);
        add_child($$, $1);
    }
    | LEIA IDENTIFICADOR ';' { /* Comando leia */
        $$ = create_node(NODE_CMD_LEIA, yylineno);
        $$->string_val = strdup($2);
    }
    | ESCREVA Expr ';' { /* Comando escreva para expressoes */
        $$ = create_node(NODE_CMD_ESCREVA_EXPR, yylineno);
        add_child($$, $2);
    }
    | ESCREVA CADEIACARACTERES ';' { /* Comando escreva string direta */
        $$ = create_node(NODE_CMD_ESCREVA_STRING, yylineno);
        $$->string_val = strdup($2);
    }
    | NOVALINHA ';' { /* Comando novalinha */
        $$ = create_node(NODE_CMD_NOVALINHA, yylineno);
    }
    | SE '(' Expr ')' ENTAO Comando FIMSE { /* Condicional IF Simples */
        $$ = create_node(NODE_CMD_IF, yylineno);
        add_child($$, $3); /* A expressao booleana/condição */
        add_child($$, $6); /* O comando ou bloco executado */
    }
    | SE '(' Expr ')' ENTAO Comando SENAO Comando FIMSE { /* Condicional IF-ELSE */
        $$ = create_node(NODE_CMD_IF, yylineno);
        add_child($$, $3); /* Condição */
        add_child($$, $6); /* Código do Entao */
        add_child($$, $8); /* Código do Senao */
    }
    | ENQUANTO '(' Expr ')' Comando { /* Laço WHILE */
        $$ = create_node(NODE_CMD_WHILE, yylineno);
        add_child($$, $3);
        add_child($$, $5);
    }
    | Bloco { /* Blocos podem ser aninhados, e tratados como Comando na linguagem. */
        $$ = $1;
    }
    ;

Expr
    /* Atribuição é uma das expressões. (Prioridade Baixa) */
    : OrExpr { $$ = $1; }
    | IDENTIFICADOR '=' Expr {
        $$ = create_node(NODE_ASSIGN, yylineno);
        add_child($$, create_var_node($1, yylineno)); /* L-Value */
        add_child($$, $3);                            /* R-Value */
    }
    ;

OrExpr
    /* Expressões Relacionais - Lógica OR || */
    : OrExpr OU AndExpr {
        $$ = create_bin_op_node(OP_OU, $1, $3, yylineno);
    }
    | AndExpr { $$ = $1; }
    ;

AndExpr
    /* Expressões Relacionais - Lógica AND & */
    : AndExpr E EqExpr {
        $$ = create_bin_op_node(OP_E, $1, $3, yylineno);
    }
    | EqExpr { $$ = $1; }
    ;

EqExpr
    /* Operadores de Igualdade (==, !=) */
    : EqExpr IGUAL DesigExpr {
        $$ = create_bin_op_node(OP_IGUAL, $1, $3, yylineno);
    }
    | EqExpr DIFERENTE DesigExpr {
        $$ = create_bin_op_node(OP_DIFERENTE, $1, $3, yylineno);
    }
    | DesigExpr { $$ = $1; }
    ;

DesigExpr
    /* Operadores Relacionais de Designação (<, >, >=, <=) */
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
    /* Operações Aditivas (+, -) - (Prioridade Média) */
    : AddExpr '+' MulExpr {
        $$ = create_bin_op_node(OP_ADD, $1, $3, yylineno);
    }
    | AddExpr '-' MulExpr {
        $$ = create_bin_op_node(OP_SUB, $1, $3, yylineno);
    }
    | MulExpr { $$ = $1; }
    ;

MulExpr
    /* Operações Multiplicativas (*, /) - (Prioridade Alta) */
    : MulExpr '*' UnExpr {
        $$ = create_bin_op_node(OP_MUL, $1, $3, yylineno);
    }
    | MulExpr '/' UnExpr {
        $$ = create_bin_op_node(OP_DIV, $1, $3, yylineno);
    }
    | UnExpr { $$ = $1; }
    ;

UnExpr
    /* Operações Unárias -, ! - (Prioridade Muito Alta) */
    : '-' PrimExpr {
        $$ = create_un_op_node(OP_NEG, $2, yylineno);
    }
    | '!' PrimExpr {
        $$ = create_un_op_node(OP_NOT, $2, yylineno);
    }
    | PrimExpr { $$ = $1; }
    ;

PrimExpr
    /* Expressões Primárias (Maior prioridade: resolve parenteses ou literais) */
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
        $$ = $2; /* Eleva os parênteses resolvendo a expr de dentro primeiro */
    }
    ;

%%

/* Função Invocada em caso de erro na Regra Sintática pelo Bison. */
void yyerror(const char *msg) {
    (void)msg; /* Evita Warning por nâo utilizar parâmetro no C */
    /* Imprime de forma crua, parando compilação (return 1). */
    printf("ERRO: SINTATICO NA LINHA %d\n", yylineno);
    exit(1);
}
