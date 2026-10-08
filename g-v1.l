/*
 * =============================================================================
 * g-v1.l — Analisador Léxico (Flex)
 * =============================================================================
 *
 * RESPONSABILIDADE:
 *   Reconhecer "palavras" (tokens) no texto do código-fonte.
 *   Lê o arquivo caractere a caractere e agrupa em tokens lógicos como
 *   'INTCONST', 'IDENTIFICADOR', 'SE', 'OU'.
 *
 * RELAÇÃO COM OUTROS ARQUIVOS:
 *   - É consumido pelo Bison (`g-v1.y`). A função gerada aqui `yylex()`
 *     é chamada pelo parser sempre que precisa do próximo token.
 *   - As definições dos tokens (ex: INTCONST, SE) vêm do cabeçalho
 *     `g-v1.tab.h` (gerado automaticamente pelo Bison).
 * =============================================================================
 */

%top{
#define _DEFAULT_SOURCE
}

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "g-v1.tab.h" /* Definição de tokens gerada pelo Bison */

/* Variáveis para acompanhar linhas onde começaram blocos, p/ mensagens de erro mais precisas */
int comment_start_line = 0;
int string_start_line = 0;

/* Converte os caracteres literais escapados de 'A' e '\n' para chars do C */
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

/*
 * Configurações do FLEX:
 * yylineno: Ativa a variável global yylineno (que conta o nº da linha atual)
 * noyywrap: Diz que o lexer para ao chegar no EOF (fim do arquivo)
 */
%option yylineno
%option noyywrap

/*
 * Estados (states) do Lexer.
 * O Lexer pode estar num estado "normal", ou em estados exclusivos (%x)
 * como lendo um comentário (COMMENT) ou lendo uma string (STRING_STATE).
 */
%x COMMENT
%x STRING_STATE

%%

/* --------------------------------------------------------------------------
 * Regras: Padrão Regex (Esquerda) -> Ação em C (Direita)
 * -------------------------------------------------------------------------- */

/* ==== 1. Tratamento de Comentários em Bloco /* ... */ ==== */

/* Entra no estado de comentário ao ver "/*" */
"/*" {
    comment_start_line = yylineno;
    BEGIN(COMMENT);
}

<COMMENT>{
    /* Sai do estado de comentário */
    "*/" {
        BEGIN(INITIAL);
    }
    \n { } /* ignora \n */
    . { }  /* ignora qualquer caractere dentro do comentário */
    <<EOF>> {
        /* Fim de arquivo e não fecharam o comentário */
        printf("ERRO: COMENTARIO NAO TERMINA %d\n", comment_start_line);
        exit(1);
    }
}

/* ==== 2. Tratamento de Cadeias de Caracteres (Strings "...") ==== */

/* Entra no estado STRING_STATE ao ver " */
"\"" {
    string_start_line = yylineno;
    yymore(); /* Instrui para juntar o que leu agora (") com a próxima leitura */
    BEGIN(STRING_STATE);
}

<STRING_STATE>{
    /* Fecha as aspas: finaliza e retorna o token */
    "\"" {
        BEGIN(INITIAL);
        yylval.str = strdup(yytext); /* Grava o valor string no union yyval */
        return CADEIACARACTERES;
    }
    /* String não pode ter quebra de linha sem aspas, é erro */
    \n {
        printf("ERRO: CADEIA DE CARACTERES OCUPA MAIS DE UMA LINHA %d\n", string_start_line);
        exit(1);
    }
    /* Fim de arquivo sem fechar aspas */
    <<EOF>> {
        printf("ERRO: CADEIA DE CARACTERES OCUPA MAIS DE UMA LINHA %d\n", string_start_line);
        exit(1);
    }
    /* Lê caracter escapado (ex: \n) ou normal e junta na leitura via yymore() */
    \\. { yymore(); }
    [^"\n\\]+ { yymore(); }
}

/* ==== 3. Espaços Brancos ==== */
[ \t\r\n]+ { /* Ignorar espaços em branco, tabulações e quebras de linha */ }

/* ==== 4. Palavras Reservadas da Linguagem ==== */
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

/* ==== 5. Operadores ==== */
"||"            { return OU; }
"&"             { return E; }
"=="            { return IGUAL; }
"!="            { return DIFERENTE; }
">="            { return MAIORIGUAL; }
"<="            { return MENORIGUAL; }

/* ==== 6. Símbolos Avulsos ==== */
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

/* ==== 7. Identificadores (Nomes de variáveis) ==== */
/* Letra ou _, seguido de zero ou mais Letras, Numeros ou _ */
[a-zA-Z_][a-zA-Z0-9_]* {
    yylval.str = strdup(yytext); /* Copia o lexema (nome) */
    return IDENTIFICADOR;
}

/* ==== 8. Literais Numéricos (Inteiros) ==== */
[0-9]+ {
    yylval.intval = atoi(yytext); /* Converte para número na linguagem C */
    return INTCONST;
}

/* ==== 9. Constantes Caractere Único ==== */
'([^'\n\\]|\\.)' {
    yylval.charval = parse_char_const(yytext);
    return CARCONST;
}

/* ==== 10. Regra de Fallback (Erro Léxico) ==== */
/* Qualquer caractere que não casou com as regras anteriores (. sem ser \n) gera erro */
. {
    printf("ERRO: CARACTERE INVALIDO %d\n", yylineno);
    exit(1);
}

%%
