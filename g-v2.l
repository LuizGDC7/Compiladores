%top{
#define _DEFAULT_SOURCE
}

%{
/*
 * [G-V2] Analisador léxico, criado a partir de g-v1.l como recomenda o enunciado.
 * Alterações: tokens global, funcao, retorne, '[' e ']'; token ATRIBUICAO;
 * INTCONST com sinal opcional; localização (linha) de cada token para o Bison.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "g-v2.tab.h"

int comment_start_line = 0;
int string_start_line = 0;

/* [G-V2] Último token devolvido; usado para decidir o papel de + e - (ver abaixo). */
static int last_token = 0;

/* [G-V2] Toda devolução de token passa por aqui para registrar last_token. */
#define RET(tok) do { last_token = (tok); return (tok); } while (0)

/* [G-V2] Informa ao Bison a linha do token, usada em @1, @2... na gramática. */
#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno;

/* [G-V2] Verdadeiro se o token anterior pode ser o fim de um operando. */
static int previous_ends_operand(void) {
    return last_token == IDENTIFICADOR || last_token == INTCONST ||
           last_token == CARCONST || last_token == ')' || last_token == ']';
}

static char parse_char_const(const char *text) {
    if (text[1] == '\\') {
        switch (text[2]) {
            case 'n': return '\n';
            case 't': return '\t';
            case '0': return '\0';
            case '\\': return '\\';
            case '\'': return '\'';
            default: return text[2];
        }
    }
    return text[1];
}
%}

%option yylineno
%option noyywrap
%option nounput
%option noinput

%x COMMENT
%x STRING_STATE

%%

"/*" {
    comment_start_line = yylineno;
    BEGIN(COMMENT);
}

<COMMENT>{
    "*/" {
        BEGIN(INITIAL);
    }
    \n { }
    . { }
    <<EOF>> {
        /* [G-V2] Texto ajustado para o exigido pelo enunciado. */
        printf("ERRO: COMENTÁRIO NAO TERMINA %d\n", comment_start_line);
        exit(1);
    }
}

"\"" {
    string_start_line = yylineno;
    yymore();
    BEGIN(STRING_STATE);
}

<STRING_STATE>{
    "\"" {
        BEGIN(INITIAL);
        yylval.str = strdup(yytext);
        RET(CADEIACARACTERES);
    }
    \n {
        printf("ERRO: CADEIA DE CARACTERES OCUPA MAIS DE UMA LINHA %d\n", string_start_line);
        exit(1);
    }
    <<EOF>> {
        printf("ERRO: CADEIA DE CARACTERES OCUPA MAIS DE UMA LINHA %d\n", string_start_line);
        exit(1);
    }
    \\. { yymore(); }
    \\  { yymore(); /* [G-V2] barra no fim da linha: o \n seguinte gera o erro */ }
    [^"\n\\]+ { yymore(); }
}

[ \t\r\n]+ { /* Ignorar espaços em branco */ }

"global"        { RET(GLOBAL); }   /* [G-V2] */
"funcao"        { RET(FUNCAO); }   /* [G-V2] */
"retorne"       { RET(RETORNE); }  /* [G-V2] */
"principal"     { RET(PRINCIPAL); }
"int"           { RET(INT); }
"car"           { RET(CAR); }
"leia"          { RET(LEIA); }
"escreva"       { RET(ESCREVA); }
"novalinha"     { RET(NOVALINHA); }
"se"            { RET(SE); }
"entao"         { RET(ENTAO); }
"senao"         { RET(SENAO); }
"fimse"         { RET(FIMSE); }
"enquanto"      { RET(ENQUANTO); }

"||"            { RET(OU); }
"&"             { RET(E); }
"=="            { RET(IGUAL); }
"!="            { RET(DIFERENTE); }
">="            { RET(MAIORIGUAL); }
"<="            { RET(MENORIGUAL); }

"{"             { RET('{'); }
"}"             { RET('}'); }
"["             { RET('['); }      /* [G-V2] */
"]"             { RET(']'); }      /* [G-V2] */
":"             { RET(':'); }
";"             { RET(';'); }
","             { RET(','); }
"="             { RET(ATRIBUICAO); } /* [G-V2] nome usado na gramática do enunciado */
"<"             { RET('<'); }
">"             { RET('>'); }
"+"             { RET('+'); }
"-"             { RET('-'); }
"*"             { RET('*'); }
"/"             { RET('/'); }
"!"             { RET('!'); }
"("             { RET('('); }
")"             { RET(')'); }

[a-zA-Z_][a-zA-Z0-9_]* {
    yylval.str = strdup(yytext);
    RET(IDENTIFICADOR);
}

 /*
  * [G-V2] A Tabela 1 permite INTCONST com sinal. Porém, se "-1" fosse sempre uma
  * constante, "n-1" viraria IDENTIFICADOR INTCONST e geraria erro sintático.
  * Por isso, depois de um operando o sinal é devolvido como operador: yyless(1)
  * mantém apenas o sinal neste token e devolve os dígitos para a entrada.
  */
[+-][0-9]+ {
    if (previous_ends_operand()) {
        int sign = yytext[0];
        yyless(1);
        RET(sign);
    }
    yylval.intval = atoi(yytext);
    RET(INTCONST);
}

[0-9]+ {
    yylval.intval = atoi(yytext);
    RET(INTCONST);
}

'([^'\n\\]|\\.)' {
    yylval.charval = parse_char_const(yytext);
    RET(CARCONST);
}

. {
    /* [G-V2] Texto ajustado para o exigido pelo enunciado. */
    printf("ERRO: CARACTERE INVÁLIDO %d\n", yylineno);
    exit(1);
}

%%
