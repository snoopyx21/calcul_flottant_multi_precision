/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ID = 258,
    INTEGER = 259,
    FLOAT = 260,
    PRAGMA = 261,
    SYMBOL = 262,
    STRING = 263,
    OR = 264,
    AND = 265,
    GTE = 266,
    LTE = 267,
    IF = 268,
    ELSE = 269,
    FOR = 270,
    WHILE = 271,
    DO = 272,
    NEQ = 273,
    EQ = 274,
    PRECISION = 275,
    ROUNDING = 276,
    BACKSLASH = 277
  };
#endif
/* Tokens.  */
#define ID 258
#define INTEGER 259
#define FLOAT 260
#define PRAGMA 261
#define SYMBOL 262
#define STRING 263
#define OR 264
#define AND 265
#define GTE 266
#define LTE 267
#define IF 268
#define ELSE 269
#define FOR 270
#define WHILE 271
#define DO 272
#define NEQ 273
#define EQ 274
#define PRECISION 275
#define ROUNDING 276
#define BACKSLASH 277

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 60 "src/syntax/syntax.y" /* yacc.c:1909  */

	char  * string;
	int     value;
	double   fvalue;

	struct  pragmaExt 
	{
		enum {ROUNDING_T, PRECISION_T} type;
		union {    
            int     precision;
		    char *  rounding;
        };
    } extension;

    struct 
    {
        int precision;
        char * rounding;
    } p_extension;

    struct number
    {
        char type; // C2MP_NUM_TYPE_INTEGER = int, C2MP_NUM_TYPE_FLOAT = double
        union 
        {
          int valueInt;
          double valueFloat;
        };
    } number;

    struct
    {
        const char * name;
        int reference;
        /*union{
            int valueInt;
            double valueFloat;
        };*/
    } variable;
	
	struct expressionAST
    {
		char operator;
        union
        {
		    struct {
                struct expressionAST *e1;
		        struct expressionAST *e2;
            } expression;
            int valueInt;
            double valueFloat;
            int valueVariable;
            char *valueString;
            struct{
                char *name;
                int argnum;
                struct expressionAST *args[MAX_FCT_ARGS];
            } customFunction;
        };
	} *expressionAST;

    struct{
        int argsNum;
        struct expressionAST *tabAST[MAX_FCT_ARGS]; 
    } argumentsAST; // for functions arguments
	
	struct semiQuad
    { // quad that can have an AST expression for val1
		int assignment;
		char operator;

        /*union
        { // depending on the operator
            struct expressionAST *expression; // example : "="
            int variable; // example "if"
        };*/
        struct expressionAST *expression;

        struct semiQuad *previous;
        struct semiQuad *next;
    } *semiQuad;

    struct quadOperand
    {
        int type;
        union
        {
            int reference;
            int valueInt;
            double valueFloat;
            char *valueString;
        };
    } quadOperand;

    struct quad
    {
        int assignment; // variable to which the operation is affected
        char operator;
        char *fctName; // used if the quad represents a function
        struct quadOperand operands[MAX_FCT_ARGS];
        int operandsNum;
        struct quad *previous;
        struct quad *next;
    } *quad;


#line 205 "y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
