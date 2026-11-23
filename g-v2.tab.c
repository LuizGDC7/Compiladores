/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* "%code top" blocks.  */
#line 1 "g-v2.y"

    #define _DEFAULT_SOURCE

#line 72 "g-v2.tab.c"



/* First part of user prologue.  */
#line 12 "g-v2.y"

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

#line 116 "g-v2.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "g-v2.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_GLOBAL = 3,                     /* GLOBAL  */
  YYSYMBOL_FUNCAO = 4,                     /* FUNCAO  */
  YYSYMBOL_RETORNE = 5,                    /* RETORNE  */
  YYSYMBOL_ATRIBUICAO = 6,                 /* ATRIBUICAO  */
  YYSYMBOL_PRINCIPAL = 7,                  /* PRINCIPAL  */
  YYSYMBOL_INT = 8,                        /* INT  */
  YYSYMBOL_CAR = 9,                        /* CAR  */
  YYSYMBOL_LEIA = 10,                      /* LEIA  */
  YYSYMBOL_ESCREVA = 11,                   /* ESCREVA  */
  YYSYMBOL_NOVALINHA = 12,                 /* NOVALINHA  */
  YYSYMBOL_SE = 13,                        /* SE  */
  YYSYMBOL_ENTAO = 14,                     /* ENTAO  */
  YYSYMBOL_SENAO = 15,                     /* SENAO  */
  YYSYMBOL_FIMSE = 16,                     /* FIMSE  */
  YYSYMBOL_ENQUANTO = 17,                  /* ENQUANTO  */
  YYSYMBOL_OU = 18,                        /* OU  */
  YYSYMBOL_E = 19,                         /* E  */
  YYSYMBOL_IGUAL = 20,                     /* IGUAL  */
  YYSYMBOL_DIFERENTE = 21,                 /* DIFERENTE  */
  YYSYMBOL_MAIORIGUAL = 22,                /* MAIORIGUAL  */
  YYSYMBOL_MENORIGUAL = 23,                /* MENORIGUAL  */
  YYSYMBOL_IDENTIFICADOR = 24,             /* IDENTIFICADOR  */
  YYSYMBOL_CADEIACARACTERES = 25,          /* CADEIACARACTERES  */
  YYSYMBOL_INTCONST = 26,                  /* INTCONST  */
  YYSYMBOL_CARCONST = 27,                  /* CARCONST  */
  YYSYMBOL_28_ = 28,                       /* '['  */
  YYSYMBOL_29_ = 29,                       /* ']'  */
  YYSYMBOL_30_ = 30,                       /* ':'  */
  YYSYMBOL_31_ = 31,                       /* ';'  */
  YYSYMBOL_32_ = 32,                       /* ','  */
  YYSYMBOL_33_ = 33,                       /* '('  */
  YYSYMBOL_34_ = 34,                       /* ')'  */
  YYSYMBOL_35_ = 35,                       /* '{'  */
  YYSYMBOL_36_ = 36,                       /* '}'  */
  YYSYMBOL_37_ = 37,                       /* '<'  */
  YYSYMBOL_38_ = 38,                       /* '>'  */
  YYSYMBOL_39_ = 39,                       /* '+'  */
  YYSYMBOL_40_ = 40,                       /* '-'  */
  YYSYMBOL_41_ = 41,                       /* '*'  */
  YYSYMBOL_42_ = 42,                       /* '/'  */
  YYSYMBOL_43_ = 43,                       /* '!'  */
  YYSYMBOL_YYACCEPT = 44,                  /* $accept  */
  YYSYMBOL_Programa = 45,                  /* Programa  */
  YYSYMBOL_DeclVarGlobais = 46,            /* DeclVarGlobais  */
  YYSYMBOL_VarSection = 47,                /* VarSection  */
  YYSYMBOL_ListaDeclVar = 48,              /* ListaDeclVar  */
  YYSYMBOL_ListVar = 49,                   /* ListVar  */
  YYSYMBOL_DeclFunc = 50,                  /* DeclFunc  */
  YYSYMBOL_ListaFuncoes = 51,              /* ListaFuncoes  */
  YYSYMBOL_ListaParametros = 52,           /* ListaParametros  */
  YYSYMBOL_ListaParametrosTail = 53,       /* ListaParametrosTail  */
  YYSYMBOL_DeclPrograma = 54,              /* DeclPrograma  */
  YYSYMBOL_Bloco = 55,                     /* Bloco  */
  YYSYMBOL_Tipo = 56,                      /* Tipo  */
  YYSYMBOL_ListaComando = 57,              /* ListaComando  */
  YYSYMBOL_Comando = 58,                   /* Comando  */
  YYSYMBOL_Expr = 59,                      /* Expr  */
  YYSYMBOL_LValueExpr = 60,                /* LValueExpr  */
  YYSYMBOL_OrExpr = 61,                    /* OrExpr  */
  YYSYMBOL_AndExpr = 62,                   /* AndExpr  */
  YYSYMBOL_EqExpr = 63,                    /* EqExpr  */
  YYSYMBOL_DesigExpr = 64,                 /* DesigExpr  */
  YYSYMBOL_AddExpr = 65,                   /* AddExpr  */
  YYSYMBOL_MulExpr = 66,                   /* MulExpr  */
  YYSYMBOL_UnExpr = 67,                    /* UnExpr  */
  YYSYMBOL_PrimExpr = 68,                  /* PrimExpr  */
  YYSYMBOL_ListExpr = 69                   /* ListExpr  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   176

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  44
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  26
/* YYNRULES -- Number of rules.  */
#define YYNRULES  73
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  163

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   282


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    43,     2,     2,     2,     2,     2,     2,
      33,    34,    41,    39,    32,    40,     2,    42,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    30,    31,
      37,     2,    38,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    28,     2,    29,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    35,     2,    36,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    82,    82,    93,    94,    99,   103,   108,   116,   121,
     126,   131,   140,   145,   149,   154,   158,   159,   164,   169,
     174,   179,   188,   192,   197,   205,   206,   210,   214,   220,
     223,   227,   231,   235,   239,   243,   246,   251,   257,   262,
     269,   274,   278,   282,   289,   292,   296,   299,   303,   306,
     309,   313,   316,   319,   322,   325,   329,   332,   335,   339,
     342,   345,   349,   352,   355,   360,   364,   368,   372,   376,
     379,   382,   389,   393
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "GLOBAL", "FUNCAO",
  "RETORNE", "ATRIBUICAO", "PRINCIPAL", "INT", "CAR", "LEIA", "ESCREVA",
  "NOVALINHA", "SE", "ENTAO", "SENAO", "FIMSE", "ENQUANTO", "OU", "E",
  "IGUAL", "DIFERENTE", "MAIORIGUAL", "MENORIGUAL", "IDENTIFICADOR",
  "CADEIACARACTERES", "INTCONST", "CARCONST", "'['", "']'", "':'", "';'",
  "','", "'('", "')'", "'{'", "'}'", "'<'", "'>'", "'+'", "'-'", "'*'",
  "'/'", "'!'", "$accept", "Programa", "DeclVarGlobais", "VarSection",
  "ListaDeclVar", "ListVar", "DeclFunc", "ListaFuncoes", "ListaParametros",
  "ListaParametrosTail", "DeclPrograma", "Bloco", "Tipo", "ListaComando",
  "Comando", "Expr", "LValueExpr", "OrExpr", "AndExpr", "EqExpr",
  "DesigExpr", "AddExpr", "MulExpr", "UnExpr", "PrimExpr", "ListExpr", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-128)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-44)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
       7,   -15,    16,    15,     4,  -128,  -128,     9,    36,    23,
      40,    60,    59,    18,  -128,    66,     4,  -128,    63,    72,
      21,    84,  -128,    93,  -128,  -128,  -128,    92,   101,    58,
     102,    33,    96,    95,    97,     2,  -128,  -128,  -128,    58,
      -9,    -9,  -128,    98,    21,   100,   123,   114,   116,    54,
     -16,    49,    61,  -128,  -128,    21,   104,     4,    -1,    99,
    -128,   106,   110,   108,   109,   111,  -128,    58,    58,    58,
      53,   107,    34,  -128,  -128,  -128,  -128,  -128,    58,    73,
      73,    73,    73,    73,    73,    73,    73,    73,    73,    73,
      73,   112,     4,  -128,   115,    63,   113,  -128,    58,  -128,
    -128,  -128,   117,   118,   120,  -128,  -128,    31,  -128,    58,
    -128,   116,    54,   -16,   -16,    49,    49,    49,    49,    61,
      61,  -128,  -128,  -128,  -128,   124,   121,    63,   126,   131,
      21,   140,    58,  -128,   127,    63,   101,    18,  -128,    21,
    -128,  -128,  -128,   125,  -128,   134,    79,   101,   128,   130,
      21,  -128,  -128,   101,  -128,   144,   129,  -128,   132,    63,
      18,   134,  -128
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       4,     0,     0,    13,     0,     3,     1,     0,     0,    10,
       0,     0,     0,     0,     2,     0,     0,     5,     0,     0,
       0,     0,    22,     0,     8,    25,    26,     0,    17,     0,
       0,     0,     0,     0,     0,    68,    70,    69,    29,     0,
       0,     0,    39,     0,    27,     0,     0,    41,    45,    47,
      50,    55,    58,    61,    64,     0,    11,     7,     0,     0,
      16,     0,    43,     0,     0,     0,    35,     0,     0,     0,
       0,     0,    68,    62,    63,    23,    28,    30,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     6,     0,     0,     0,    31,     0,    32,
      34,    33,     0,     0,     0,    66,    72,     0,    71,     0,
      40,    44,    46,    48,    49,    53,    54,    51,    52,    56,
      57,    59,    60,    24,     9,     0,    18,     0,     0,     0,
       0,    67,     0,    65,     0,     0,     0,     0,    42,     0,
      38,    73,    67,    19,    20,    15,     0,     0,     0,     0,
       0,    36,    21,    17,    12,     0,     0,    37,     0,     0,
       0,    15,    14
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -128,  -128,  -128,   149,   119,   -14,  -128,     3,    12,  -127,
    -128,   -13,   -91,   -32,  -125,   -28,   136,  -128,    88,    89,
      30,    24,    27,    28,    80,  -128
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     2,     3,    21,    10,    11,     8,   149,    59,    60,
      14,    42,    27,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,   107
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      22,    61,    24,    65,   126,   140,    83,    84,   -43,   144,
       1,    71,    76,     4,   146,    72,     6,    36,    37,     7,
     152,    85,    86,    91,    39,   155,    29,    94,     9,    95,
      69,    30,    31,    32,    33,    70,   137,    12,    34,   102,
     103,   104,   106,    13,   143,    35,     4,    36,    37,     4,
     110,    15,    38,    20,    39,    16,    20,    35,    64,    36,
      37,    40,   109,   132,    41,   133,    39,    70,   160,    17,
     128,    25,    26,    40,    81,    82,    41,    35,   124,    36,
      37,   134,    35,    19,    36,    37,    39,   105,    87,    88,
      18,    39,    23,    40,   150,   151,    41,    72,    40,    36,
      37,    41,    89,    90,   141,    28,    39,   115,   116,   117,
     118,   113,   114,    40,   119,   120,    41,   121,   122,    55,
      73,    74,    56,    57,   145,    58,    62,    66,    67,    78,
      68,    77,    79,    96,    75,    80,    92,    97,    98,    99,
     100,   108,   101,   127,   125,   139,   -42,   161,   123,   131,
       5,   129,   130,   136,   135,   138,   142,   147,   148,   154,
     157,   153,   159,   158,   162,   156,    63,   111,     0,   112,
       0,     0,     0,     0,     0,     0,    93
};

static const yytype_int16 yycheck[] =
{
      13,    29,    16,    31,    95,   130,    22,    23,     6,   136,
       3,    39,    44,    28,   139,    24,     0,    26,    27,     4,
     147,    37,    38,    55,    33,   150,     5,    28,    24,    30,
      28,    10,    11,    12,    13,    33,   127,    28,    17,    67,
      68,    69,    70,     7,   135,    24,    28,    26,    27,    28,
      78,    28,    31,    35,    33,    32,    35,    24,    25,    26,
      27,    40,    28,    32,    43,    34,    33,    33,   159,    29,
      98,     8,     9,    40,    20,    21,    43,    24,    92,    26,
      27,   109,    24,    24,    26,    27,    33,    34,    39,    40,
      30,    33,    26,    40,    15,    16,    43,    24,    40,    26,
      27,    43,    41,    42,   132,    33,    33,    83,    84,    85,
      86,    81,    82,    40,    87,    88,    43,    89,    90,    35,
      40,    41,    29,    31,   137,    24,    24,    31,    33,     6,
      33,    31,    18,    34,    36,    19,    32,    31,    28,    31,
      31,    34,    31,    30,    29,    14,     6,   160,    36,    29,
       1,    34,    34,    32,    30,    29,    29,    32,    24,    29,
      16,    33,    30,    34,   161,   153,    30,    79,    -1,    80,
      -1,    -1,    -1,    -1,    -1,    -1,    57
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,    45,    46,    28,    47,     0,     4,    50,    24,
      48,    49,    28,     7,    54,    28,    32,    29,    30,    24,
      35,    47,    55,    26,    49,     8,     9,    56,    33,     5,
      10,    11,    12,    13,    17,    24,    26,    27,    31,    33,
      40,    43,    55,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    35,    29,    31,    24,    52,
      53,    59,    24,    60,    25,    59,    31,    33,    33,    28,
      33,    59,    24,    68,    68,    36,    57,    31,     6,    18,
      19,    20,    21,    22,    23,    37,    38,    39,    40,    41,
      42,    57,    32,    48,    28,    30,    34,    31,    28,    31,
      31,    31,    59,    59,    59,    34,    59,    69,    34,    28,
      59,    62,    63,    64,    64,    65,    65,    65,    65,    66,
      66,    67,    67,    36,    49,    29,    56,    30,    59,    34,
      34,    29,    32,    34,    59,    30,    32,    56,    29,    14,
      58,    59,    29,    56,    53,    55,    58,    32,    24,    51,
      15,    16,    53,    33,    29,    58,    52,    16,    34,    30,
      56,    55,    51
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    44,    45,    46,    46,    47,    48,    48,    49,    49,
      49,    49,    50,    50,    51,    51,    52,    52,    53,    53,
      53,    53,    54,    55,    55,    56,    56,    57,    57,    58,
      58,    58,    58,    58,    58,    58,    58,    58,    58,    58,
      59,    59,    60,    60,    61,    61,    62,    62,    63,    63,
      63,    64,    64,    64,    64,    64,    65,    65,    65,    66,
      66,    66,    67,    67,    67,    68,    68,    68,    68,    68,
      68,    68,    69,    69
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     3,     2,     0,     3,     5,     4,     3,     6,
       1,     4,    11,     0,     8,     0,     1,     0,     3,     5,
       5,     7,     2,     3,     4,     1,     1,     1,     2,     1,
       2,     3,     3,     3,     3,     2,     7,     9,     5,     1,
       3,     1,     4,     1,     3,     1,     3,     1,     3,     3,
       1,     3,     3,     3,     3,     1,     3,     3,     1,     3,
       3,     1,     2,     2,     1,     4,     3,     4,     1,     1,
       1,     3,     1,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]));
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* Programa: DeclVarGlobais DeclFunc DeclPrograma  */
#line 82 "g-v2.y"
                                           {
        (yyval.node) = create_node(NODE_PROGRAM, (yylsp[0]).first_line);
        add_child((yyval.node), (yyvsp[-2].node));
        add_child((yyval.node), (yyvsp[-1].node));
        add_child((yyval.node), (yyvsp[0].node));
        ast_root = (yyval.node);
    }
#line 1399 "g-v2.tab.c"
    break;

  case 3: /* DeclVarGlobais: GLOBAL VarSection  */
#line 93 "g-v2.y"
                        { (yyval.node) = (yyvsp[0].node); }
#line 1405 "g-v2.tab.c"
    break;

  case 4: /* DeclVarGlobais: %empty  */
#line 94 "g-v2.y"
                  { (yyval.node) = create_node(NODE_VAR_DECL_LIST, yylineno); }
#line 1411 "g-v2.tab.c"
    break;

  case 5: /* VarSection: '[' ListaDeclVar ']'  */
#line 99 "g-v2.y"
                           { (yyval.node) = (yyvsp[-1].node); }
#line 1417 "g-v2.tab.c"
    break;

  case 6: /* ListaDeclVar: ListVar ':' Tipo ';' ListaDeclVar  */
#line 103 "g-v2.y"
                                        {
        apply_type((yyvsp[-4].node), (yyvsp[-2].type));
        move_children((yyvsp[-4].node), (yyvsp[0].node));
        (yyval.node) = (yyvsp[-4].node);
    }
#line 1427 "g-v2.tab.c"
    break;

  case 7: /* ListaDeclVar: ListVar ':' Tipo ';'  */
#line 108 "g-v2.y"
                           {
        apply_type((yyvsp[-3].node), (yyvsp[-1].type));
        (yyval.node) = (yyvsp[-3].node);
    }
#line 1436 "g-v2.tab.c"
    break;

  case 8: /* ListVar: IDENTIFICADOR ',' ListVar  */
#line 116 "g-v2.y"
                                {
        (yyval.node) = prepend_to_list(NODE_VAR_DECL_LIST,
                             create_var_decl_node((yyvsp[-2].str), 0, 0, (yylsp[-2]).first_line), (yyvsp[0].node), (yylsp[-2]).first_line);
        free((yyvsp[-2].str));
    }
#line 1446 "g-v2.tab.c"
    break;

  case 9: /* ListVar: IDENTIFICADOR '[' INTCONST ']' ',' ListVar  */
#line 121 "g-v2.y"
                                                 {
        (yyval.node) = prepend_to_list(NODE_VAR_DECL_LIST,
                             create_var_decl_node((yyvsp[-5].str), 1, (yyvsp[-3].intval), (yylsp[-5]).first_line), (yyvsp[0].node), (yylsp[-5]).first_line);
        free((yyvsp[-5].str));
    }
#line 1456 "g-v2.tab.c"
    break;

  case 10: /* ListVar: IDENTIFICADOR  */
#line 126 "g-v2.y"
                    {
        (yyval.node) = prepend_to_list(NODE_VAR_DECL_LIST,
                             create_var_decl_node((yyvsp[0].str), 0, 0, (yylsp[0]).first_line), NULL, (yylsp[0]).first_line);
        free((yyvsp[0].str));
    }
#line 1466 "g-v2.tab.c"
    break;

  case 11: /* ListVar: IDENTIFICADOR '[' INTCONST ']'  */
#line 131 "g-v2.y"
                                     {
        (yyval.node) = prepend_to_list(NODE_VAR_DECL_LIST,
                             create_var_decl_node((yyvsp[-3].str), 1, (yyvsp[-1].intval), (yylsp[-3]).first_line), NULL, (yylsp[-3]).first_line);
        free((yyvsp[-3].str));
    }
#line 1476 "g-v2.tab.c"
    break;

  case 12: /* DeclFunc: FUNCAO '[' IDENTIFICADOR '(' ListaParametros ')' ':' Tipo Bloco ListaFuncoes ']'  */
#line 140 "g-v2.y"
                                                                                       {
        ASTNode *func = create_func_node((yyvsp[-8].str), (yyvsp[-3].type), (yyvsp[-6].node), (yyvsp[-2].node), (yylsp[-8]).first_line);
        (yyval.node) = prepend_to_list(NODE_FUNC_LIST, func, (yyvsp[-1].node), (yylsp[-10]).first_line);
        free((yyvsp[-8].str));
    }
#line 1486 "g-v2.tab.c"
    break;

  case 13: /* DeclFunc: %empty  */
#line 145 "g-v2.y"
                  { (yyval.node) = create_node(NODE_FUNC_LIST, yylineno); }
#line 1492 "g-v2.tab.c"
    break;

  case 14: /* ListaFuncoes: IDENTIFICADOR '(' ListaParametros ')' ':' Tipo Bloco ListaFuncoes  */
#line 149 "g-v2.y"
                                                                        {
        ASTNode *func = create_func_node((yyvsp[-7].str), (yyvsp[-2].type), (yyvsp[-5].node), (yyvsp[-1].node), (yylsp[-7]).first_line);
        (yyval.node) = prepend_to_list(NODE_FUNC_LIST, func, (yyvsp[0].node), (yylsp[-7]).first_line);
        free((yyvsp[-7].str));
    }
#line 1502 "g-v2.tab.c"
    break;

  case 15: /* ListaFuncoes: %empty  */
#line 154 "g-v2.y"
                  { (yyval.node) = create_node(NODE_FUNC_LIST, yylineno); }
#line 1508 "g-v2.tab.c"
    break;

  case 16: /* ListaParametros: ListaParametrosTail  */
#line 158 "g-v2.y"
                          { (yyval.node) = (yyvsp[0].node); }
#line 1514 "g-v2.tab.c"
    break;

  case 17: /* ListaParametros: %empty  */
#line 159 "g-v2.y"
                  { (yyval.node) = create_node(NODE_PARAM_LIST, yylineno); }
#line 1520 "g-v2.tab.c"
    break;

  case 18: /* ListaParametrosTail: IDENTIFICADOR ':' Tipo  */
#line 164 "g-v2.y"
                             {
        (yyval.node) = prepend_to_list(NODE_PARAM_LIST,
                             create_param_node((yyvsp[-2].str), 0, (yyvsp[0].type), (yylsp[-2]).first_line), NULL, (yylsp[-2]).first_line);
        free((yyvsp[-2].str));
    }
#line 1530 "g-v2.tab.c"
    break;

  case 19: /* ListaParametrosTail: IDENTIFICADOR '[' ']' ':' Tipo  */
#line 169 "g-v2.y"
                                     {
        (yyval.node) = prepend_to_list(NODE_PARAM_LIST,
                             create_param_node((yyvsp[-4].str), 1, (yyvsp[0].type), (yylsp[-4]).first_line), NULL, (yylsp[-4]).first_line);
        free((yyvsp[-4].str));
    }
#line 1540 "g-v2.tab.c"
    break;

  case 20: /* ListaParametrosTail: IDENTIFICADOR ':' Tipo ',' ListaParametrosTail  */
#line 174 "g-v2.y"
                                                     {
        (yyval.node) = prepend_to_list(NODE_PARAM_LIST,
                             create_param_node((yyvsp[-4].str), 0, (yyvsp[-2].type), (yylsp[-4]).first_line), (yyvsp[0].node), (yylsp[-4]).first_line);
        free((yyvsp[-4].str));
    }
#line 1550 "g-v2.tab.c"
    break;

  case 21: /* ListaParametrosTail: IDENTIFICADOR '[' ']' ':' Tipo ',' ListaParametrosTail  */
#line 179 "g-v2.y"
                                                             {
        (yyval.node) = prepend_to_list(NODE_PARAM_LIST,
                             create_param_node((yyvsp[-6].str), 1, (yyvsp[-2].type), (yylsp[-6]).first_line), (yyvsp[0].node), (yylsp[-6]).first_line);
        free((yyvsp[-6].str));
    }
#line 1560 "g-v2.tab.c"
    break;

  case 22: /* DeclPrograma: PRINCIPAL Bloco  */
#line 188 "g-v2.y"
                      { (yyval.node) = (yyvsp[0].node); }
#line 1566 "g-v2.tab.c"
    break;

  case 23: /* Bloco: '{' ListaComando '}'  */
#line 192 "g-v2.y"
                           {
        (yyval.node) = create_node(NODE_BLOCK, (yylsp[-2]).first_line);
        add_child((yyval.node), create_node(NODE_VAR_DECL_LIST, (yylsp[-2]).first_line));
        add_child((yyval.node), (yyvsp[-1].node));
    }
#line 1576 "g-v2.tab.c"
    break;

  case 24: /* Bloco: VarSection '{' ListaComando '}'  */
#line 197 "g-v2.y"
                                      {
        (yyval.node) = create_node(NODE_BLOCK, (yylsp[-3]).first_line);
        add_child((yyval.node), (yyvsp[-3].node));
        add_child((yyval.node), (yyvsp[-1].node));
    }
#line 1586 "g-v2.tab.c"
    break;

  case 25: /* Tipo: INT  */
#line 205 "g-v2.y"
          { (yyval.type) = TYPE_INT; }
#line 1592 "g-v2.tab.c"
    break;

  case 26: /* Tipo: CAR  */
#line 206 "g-v2.y"
          { (yyval.type) = TYPE_CAR; }
#line 1598 "g-v2.tab.c"
    break;

  case 27: /* ListaComando: Comando  */
#line 210 "g-v2.y"
              {
        (yyval.node) = create_node(NODE_CMD_LIST, (yylsp[0]).first_line);
        add_child((yyval.node), (yyvsp[0].node));
    }
#line 1607 "g-v2.tab.c"
    break;

  case 28: /* ListaComando: Comando ListaComando  */
#line 214 "g-v2.y"
                           {
        (yyval.node) = prepend_to_list(NODE_CMD_LIST, (yyvsp[-1].node), (yyvsp[0].node), (yylsp[-1]).first_line);
    }
#line 1615 "g-v2.tab.c"
    break;

  case 29: /* Comando: ';'  */
#line 220 "g-v2.y"
          {
        (yyval.node) = create_node(NODE_CMD_EXPR, (yylsp[0]).first_line);
    }
#line 1623 "g-v2.tab.c"
    break;

  case 30: /* Comando: Expr ';'  */
#line 223 "g-v2.y"
               {
        (yyval.node) = create_node(NODE_CMD_EXPR, (yylsp[-1]).first_line);
        add_child((yyval.node), (yyvsp[-1].node));
    }
#line 1632 "g-v2.tab.c"
    break;

  case 31: /* Comando: RETORNE Expr ';'  */
#line 227 "g-v2.y"
                       { /* [G-V2] */
        (yyval.node) = create_node(NODE_CMD_RETURN, (yylsp[-2]).first_line);
        add_child((yyval.node), (yyvsp[-1].node));
    }
#line 1641 "g-v2.tab.c"
    break;

  case 32: /* Comando: LEIA LValueExpr ';'  */
#line 231 "g-v2.y"
                          { /* [G-V2] aceita também elemento de vetor */
        (yyval.node) = create_node(NODE_CMD_LEIA, (yylsp[-2]).first_line);
        add_child((yyval.node), (yyvsp[-1].node));
    }
#line 1650 "g-v2.tab.c"
    break;

  case 33: /* Comando: ESCREVA Expr ';'  */
#line 235 "g-v2.y"
                       {
        (yyval.node) = create_node(NODE_CMD_ESCREVA_EXPR, (yylsp[-2]).first_line);
        add_child((yyval.node), (yyvsp[-1].node));
    }
#line 1659 "g-v2.tab.c"
    break;

  case 34: /* Comando: ESCREVA CADEIACARACTERES ';'  */
#line 239 "g-v2.y"
                                   {
        (yyval.node) = create_node(NODE_CMD_ESCREVA_STRING, (yylsp[-2]).first_line);
        (yyval.node)->string_val = (yyvsp[-1].str);
    }
#line 1668 "g-v2.tab.c"
    break;

  case 35: /* Comando: NOVALINHA ';'  */
#line 243 "g-v2.y"
                    {
        (yyval.node) = create_node(NODE_CMD_NOVALINHA, (yylsp[-1]).first_line);
    }
#line 1676 "g-v2.tab.c"
    break;

  case 36: /* Comando: SE '(' Expr ')' ENTAO Comando FIMSE  */
#line 246 "g-v2.y"
                                          {
        (yyval.node) = create_node(NODE_CMD_IF, (yylsp[-6]).first_line);
        add_child((yyval.node), (yyvsp[-4].node));
        add_child((yyval.node), (yyvsp[-1].node));
    }
#line 1686 "g-v2.tab.c"
    break;

  case 37: /* Comando: SE '(' Expr ')' ENTAO Comando SENAO Comando FIMSE  */
#line 251 "g-v2.y"
                                                        {
        (yyval.node) = create_node(NODE_CMD_IF, (yylsp[-8]).first_line);
        add_child((yyval.node), (yyvsp[-6].node));
        add_child((yyval.node), (yyvsp[-3].node));
        add_child((yyval.node), (yyvsp[-1].node));
    }
#line 1697 "g-v2.tab.c"
    break;

  case 38: /* Comando: ENQUANTO '(' Expr ')' Comando  */
#line 257 "g-v2.y"
                                    {
        (yyval.node) = create_node(NODE_CMD_WHILE, (yylsp[-4]).first_line);
        add_child((yyval.node), (yyvsp[-2].node));
        add_child((yyval.node), (yyvsp[0].node));
    }
#line 1707 "g-v2.tab.c"
    break;

  case 39: /* Comando: Bloco  */
#line 262 "g-v2.y"
            {
        (yyval.node) = (yyvsp[0].node);
    }
#line 1715 "g-v2.tab.c"
    break;

  case 40: /* Expr: LValueExpr ATRIBUICAO Expr  */
#line 269 "g-v2.y"
                                 {
        (yyval.node) = create_node(NODE_ASSIGN, (yylsp[-1]).first_line);
        add_child((yyval.node), (yyvsp[-2].node));
        add_child((yyval.node), (yyvsp[0].node));
    }
#line 1725 "g-v2.tab.c"
    break;

  case 41: /* Expr: OrExpr  */
#line 274 "g-v2.y"
             { (yyval.node) = (yyvsp[0].node); }
#line 1731 "g-v2.tab.c"
    break;

  case 42: /* LValueExpr: IDENTIFICADOR '[' Expr ']'  */
#line 278 "g-v2.y"
                                 {
        (yyval.node) = create_array_access_node((yyvsp[-3].str), (yyvsp[-1].node), (yylsp[-3]).first_line);
        free((yyvsp[-3].str));
    }
#line 1740 "g-v2.tab.c"
    break;

  case 43: /* LValueExpr: IDENTIFICADOR  */
#line 282 "g-v2.y"
                    {
        (yyval.node) = create_var_node((yyvsp[0].str), (yylsp[0]).first_line);
        free((yyvsp[0].str));
    }
#line 1749 "g-v2.tab.c"
    break;

  case 44: /* OrExpr: OrExpr OU AndExpr  */
#line 289 "g-v2.y"
                        {
        (yyval.node) = create_bin_op_node(OP_OU, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line);
    }
#line 1757 "g-v2.tab.c"
    break;

  case 45: /* OrExpr: AndExpr  */
#line 292 "g-v2.y"
              { (yyval.node) = (yyvsp[0].node); }
#line 1763 "g-v2.tab.c"
    break;

  case 46: /* AndExpr: AndExpr E EqExpr  */
#line 296 "g-v2.y"
                       {
        (yyval.node) = create_bin_op_node(OP_E, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line);
    }
#line 1771 "g-v2.tab.c"
    break;

  case 47: /* AndExpr: EqExpr  */
#line 299 "g-v2.y"
             { (yyval.node) = (yyvsp[0].node); }
#line 1777 "g-v2.tab.c"
    break;

  case 48: /* EqExpr: EqExpr IGUAL DesigExpr  */
#line 303 "g-v2.y"
                             {
        (yyval.node) = create_bin_op_node(OP_IGUAL, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line);
    }
#line 1785 "g-v2.tab.c"
    break;

  case 49: /* EqExpr: EqExpr DIFERENTE DesigExpr  */
#line 306 "g-v2.y"
                                 {
        (yyval.node) = create_bin_op_node(OP_DIFERENTE, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line);
    }
#line 1793 "g-v2.tab.c"
    break;

  case 50: /* EqExpr: DesigExpr  */
#line 309 "g-v2.y"
                { (yyval.node) = (yyvsp[0].node); }
#line 1799 "g-v2.tab.c"
    break;

  case 51: /* DesigExpr: DesigExpr '<' AddExpr  */
#line 313 "g-v2.y"
                            {
        (yyval.node) = create_bin_op_node(OP_LESS, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line);
    }
#line 1807 "g-v2.tab.c"
    break;

  case 52: /* DesigExpr: DesigExpr '>' AddExpr  */
#line 316 "g-v2.y"
                            {
        (yyval.node) = create_bin_op_node(OP_GREATER, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line);
    }
#line 1815 "g-v2.tab.c"
    break;

  case 53: /* DesigExpr: DesigExpr MAIORIGUAL AddExpr  */
#line 319 "g-v2.y"
                                   {
        (yyval.node) = create_bin_op_node(OP_GE, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line);
    }
#line 1823 "g-v2.tab.c"
    break;

  case 54: /* DesigExpr: DesigExpr MENORIGUAL AddExpr  */
#line 322 "g-v2.y"
                                   {
        (yyval.node) = create_bin_op_node(OP_LE, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line);
    }
#line 1831 "g-v2.tab.c"
    break;

  case 55: /* DesigExpr: AddExpr  */
#line 325 "g-v2.y"
              { (yyval.node) = (yyvsp[0].node); }
#line 1837 "g-v2.tab.c"
    break;

  case 56: /* AddExpr: AddExpr '+' MulExpr  */
#line 329 "g-v2.y"
                          {
        (yyval.node) = create_bin_op_node(OP_ADD, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line);
    }
#line 1845 "g-v2.tab.c"
    break;

  case 57: /* AddExpr: AddExpr '-' MulExpr  */
#line 332 "g-v2.y"
                          {
        (yyval.node) = create_bin_op_node(OP_SUB, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line);
    }
#line 1853 "g-v2.tab.c"
    break;

  case 58: /* AddExpr: MulExpr  */
#line 335 "g-v2.y"
              { (yyval.node) = (yyvsp[0].node); }
#line 1859 "g-v2.tab.c"
    break;

  case 59: /* MulExpr: MulExpr '*' UnExpr  */
#line 339 "g-v2.y"
                         {
        (yyval.node) = create_bin_op_node(OP_MUL, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line);
    }
#line 1867 "g-v2.tab.c"
    break;

  case 60: /* MulExpr: MulExpr '/' UnExpr  */
#line 342 "g-v2.y"
                         {
        (yyval.node) = create_bin_op_node(OP_DIV, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line);
    }
#line 1875 "g-v2.tab.c"
    break;

  case 61: /* MulExpr: UnExpr  */
#line 345 "g-v2.y"
             { (yyval.node) = (yyvsp[0].node); }
#line 1881 "g-v2.tab.c"
    break;

  case 62: /* UnExpr: '-' PrimExpr  */
#line 349 "g-v2.y"
                   {
        (yyval.node) = create_un_op_node(OP_NEG, (yyvsp[0].node), (yylsp[-1]).first_line);
    }
#line 1889 "g-v2.tab.c"
    break;

  case 63: /* UnExpr: '!' PrimExpr  */
#line 352 "g-v2.y"
                   {
        (yyval.node) = create_un_op_node(OP_NOT, (yyvsp[0].node), (yylsp[-1]).first_line);
    }
#line 1897 "g-v2.tab.c"
    break;

  case 64: /* UnExpr: PrimExpr  */
#line 355 "g-v2.y"
               { (yyval.node) = (yyvsp[0].node); }
#line 1903 "g-v2.tab.c"
    break;

  case 65: /* PrimExpr: IDENTIFICADOR '(' ListExpr ')'  */
#line 360 "g-v2.y"
                                     {
        (yyval.node) = create_call_node((yyvsp[-3].str), (yyvsp[-1].node), (yylsp[-3]).first_line);
        free((yyvsp[-3].str));
    }
#line 1912 "g-v2.tab.c"
    break;

  case 66: /* PrimExpr: IDENTIFICADOR '(' ')'  */
#line 364 "g-v2.y"
                            {
        (yyval.node) = create_call_node((yyvsp[-2].str), NULL, (yylsp[-2]).first_line);
        free((yyvsp[-2].str));
    }
#line 1921 "g-v2.tab.c"
    break;

  case 67: /* PrimExpr: IDENTIFICADOR '[' Expr ']'  */
#line 368 "g-v2.y"
                                 {
        (yyval.node) = create_array_access_node((yyvsp[-3].str), (yyvsp[-1].node), (yylsp[-3]).first_line);
        free((yyvsp[-3].str));
    }
#line 1930 "g-v2.tab.c"
    break;

  case 68: /* PrimExpr: IDENTIFICADOR  */
#line 372 "g-v2.y"
                    {
        (yyval.node) = create_var_node((yyvsp[0].str), (yylsp[0]).first_line);
        free((yyvsp[0].str));
    }
#line 1939 "g-v2.tab.c"
    break;

  case 69: /* PrimExpr: CARCONST  */
#line 376 "g-v2.y"
               {
        (yyval.node) = create_car_node((yyvsp[0].charval), (yylsp[0]).first_line);
    }
#line 1947 "g-v2.tab.c"
    break;

  case 70: /* PrimExpr: INTCONST  */
#line 379 "g-v2.y"
               {
        (yyval.node) = create_int_node((yyvsp[0].intval), (yylsp[0]).first_line);
    }
#line 1955 "g-v2.tab.c"
    break;

  case 71: /* PrimExpr: '(' Expr ')'  */
#line 382 "g-v2.y"
                   {
        (yyval.node) = (yyvsp[-1].node);
    }
#line 1963 "g-v2.tab.c"
    break;

  case 72: /* ListExpr: Expr  */
#line 389 "g-v2.y"
           {
        (yyval.node) = create_node(NODE_EXPR_LIST, (yylsp[0]).first_line);
        add_child((yyval.node), (yyvsp[0].node));
    }
#line 1972 "g-v2.tab.c"
    break;

  case 73: /* ListExpr: ListExpr ',' Expr  */
#line 393 "g-v2.y"
                        {
        add_child((yyvsp[-2].node), (yyvsp[0].node));
        (yyval.node) = (yyvsp[-2].node);
    }
#line 1981 "g-v2.tab.c"
    break;


#line 1985 "g-v2.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 399 "g-v2.y"


void yyerror(const char *msg) {
    (void)msg;
    printf("ERRO: SINTATICO NA LINHA %d\n", yylineno);
    exit(1);
}
