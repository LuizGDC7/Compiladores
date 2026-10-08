%top{
#define _DEFAULT_SOURCE
}

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "g-v1.tab.h"

int comment_start_line = 0;
int string_start_line = 0;

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
        printf("ERRO: COMENTARIO NAO TERMINA %d\n", comment_start_line);
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
        return CADEIACARACTERES;
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
    [^"\n\\]+ { yymore(); }
}

[ \t\r\n]+ { /* Ignorar espaços em branco */ }

"principal"     { return PRINCIPAL; }
"int"           { return INT; }
"car"           { return CAR; }
"leia"          { return LEIA; }
"escreva"       { return ESCREVA; }
"novalinha"     { return NOVALINHA; }
"se"            { return SE; }
"entao"         { return ENTAO; }
"senao"         { return SENAO; }
"fimse"         { return FIMSE; }
"enquanto"      { return ENQUANTO; }

"||"            { return OU; }
"&"             { return E; }
"=="            { return IGUAL; }
"!="            { return DIFERENTE; }
">="            { return MAIORIGUAL; }
"<="            { return MENORIGUAL; }

"{"             { return '{'; }
"}"             { return '}'; }
":"             { return ':'; }
";"             { return ';'; }
","             { return ','; }
"="             { return '='; }
"<"             { return '<'; }
">"             { return '>'; }
"+"             { return '+'; }
"-"             { return '-'; }
"*"             { return '*'; }
"/"             { return '/'; }
"!"             { return '!'; }
"("             { return '('; }
")"             { return ')'; }

[a-zA-Z_][a-zA-Z0-9_]* {
    yylval.str = strdup(yytext);
    return IDENTIFICADOR;
}

[0-9]+ {
    yylval.intval = atoi(yytext);
    return INTCONST;
}

'([^'\n\\]|\\.)' {
    yylval.charval = parse_char_const(yytext);
    return CARCONST;
}

. {
    printf("ERRO: CARACTERE INVALIDO %d\n", yylineno);
    exit(1);
}

%%
