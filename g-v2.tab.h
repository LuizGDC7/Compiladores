/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_G_V2_TAB_H_INCLUDED
# define YY_YY_G_V2_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 5 "g-v2.y"

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "ast.h"

#line 56 "g-v2.tab.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    GLOBAL = 258,                  /* GLOBAL  */
    FUNCAO = 259,                  /* FUNCAO  */
    RETORNE = 260,                 /* RETORNE  */
    ATRIBUICAO = 261,              /* ATRIBUICAO  */
    PRINCIPAL = 262,               /* PRINCIPAL  */
    INT = 263,                     /* INT  */
    CAR = 264,                     /* CAR  */
    LEIA = 265,                    /* LEIA  */
    ESCREVA = 266,                 /* ESCREVA  */
    NOVALINHA = 267,               /* NOVALINHA  */
    SE = 268,                      /* SE  */
    ENTAO = 269,                   /* ENTAO  */
    SENAO = 270,                   /* SENAO  */
    FIMSE = 271,                   /* FIMSE  */
    ENQUANTO = 272,                /* ENQUANTO  */
    OU = 273,                      /* OU  */
    E = 274,                       /* E  */
    IGUAL = 275,                   /* IGUAL  */
    DIFERENTE = 276,               /* DIFERENTE  */
    MAIORIGUAL = 277,              /* MAIORIGUAL  */
    MENORIGUAL = 278,              /* MENORIGUAL  */
    IDENTIFICADOR = 279,           /* IDENTIFICADOR  */
    CADEIACARACTERES = 280,        /* CADEIACARACTERES  */
    INTCONST = 281,                /* INTCONST  */
    CARCONST = 282                 /* CARCONST  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 54 "g-v2.y"

    int intval;
    char charval;
    char *str;
    ASTNode *node;
    DataType type;

#line 108 "g-v2.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;

int yyparse (void);


#endif /* !YY_YY_G_V2_TAB_H_INCLUDED  */
