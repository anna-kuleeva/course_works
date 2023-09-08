/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
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

#ifndef YY_YY_MAKEG_TAB_H_INCLUDED
# define YY_YY_MAKEG_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IFNEQ = 258,
     IFEQ = 259,
     ELSE = 260,
     IFDEF = 261,
     IFNDEF = 262,
     ENDEF = 263,
     ENDIF = 264,
     TAB = 265,
     DIGIT = 266,
     LETTER = 267,
     UPLETTER = 268,
     COLON = 269,
     EOL = 270,
     UNIT_NAME = 271,
     RUN_COMMAND = 272,
     COMMAND = 273,
     FILE_NAME = 274,
     ANY_EQ = 275,
     DOLLAR = 276,
     OBR = 277,
     CBR = 278,
     COLEQ = 279,
     FILE_PATH = 280,
     VAR_REF = 281,
     INCLUDE = 282,
     DEFINE = 283,
     EXPORT = 284,
     SHELL_COMMAND = 285,
     TARGET_SLASH = 286,
     ANY_STRING = 287,
     TEMPLATE = 288,
     FUNCTION = 289,
     SPECIAL = 290
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2058 of yacc.c  */
#line 24 "makeg.y"

	int 	intval; 
	char 	charval;
	char*	ptrchar;


/* Line 2058 of yacc.c  */
#line 99 "makeg.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_YY_MAKEG_TAB_H_INCLUDED  */
