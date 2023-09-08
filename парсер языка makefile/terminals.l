%{
#include <stdio.h>
#include <stdlib.h>
#include "makeg.tab.h"
extern void yyerror(const char *s);
void check_brackets(char *string);
void check_slash(char* string);
extern int full_info;
extern int lines_count, comments_count, command_count;
%}

%option yylineno
%option noyywrap

DIGIT [0-9]
UNIT_NAME [_a-zA-Z\*\-\+\@\?]
VAR_REF "$"[$]?[(\{]({UNIT_NAME}|{DIGIT})+[)\}]
RUN_CMD ^(\@[^\@]*\@)?[\t]+\.\/[^\n]*
CMD_S   ^(\@[^\@]*\@)?[\t]+[^\n]*

%%
"ifeq "       { yylval.ptrchar = yytext; return IFEQ;}
"ifneq "      { yylval.ptrchar = yytext; return IFNEQ;}
"ifdef "      { yylval.ptrchar = yytext; return IFDEF;}
"ifndef "     { yylval.ptrchar = yytext; return IFNDEF;}
"else"[ ]?    { yylval.ptrchar = yytext; return ELSE;}
"endef"       { yylval.ptrchar = yytext; return ENDEF;}
"endif" 	  { yylval.ptrchar = yytext; return ENDIF;}
"include"	  { yylval.ptrchar = yytext; if (full_info) printf("Include at line %i\n", lines_count); return INCLUDE; }
"define"	  { yylval.ptrchar = yytext; return DEFINE;}
"export"	  { yylval.ptrchar = yytext; return EXPORT;}

"\\\n"[ \t]*       { yylval.ptrchar = yytext; lines_count++; return TARGET_SLASH; }
"\'"[^\']*"\'"     { yylval.ptrchar = yytext; return ANY_STRING; }
"\""[^\"]*"\""     { yylval.ptrchar = yytext; return ANY_STRING; }

{RUN_CMD}      { yylval.ptrchar = yytext; check_slash(yytext); return RUN_COMMAND;}
{CMD_S}        { yylval.ptrchar = yytext; check_slash(yytext); return COMMAND;} 

"\`"[^\`]*"\`"            { yylval.ptrchar = yytext; return SHELL_COMMAND; }
"$(shell ".*")"           { yylval.ptrchar = yytext; return SHELL_COMMAND; }


[ \t]*"#"[^\n]*[\n]?     { 
						yylval.ptrchar = yytext; 
						if (full_info == 100)  printf("Comment at line %i: %s", lines_count, yytext); 
						comments_count++; 
						lines_count++; 
					}

[":"|"!"|"?"|"+"|"::"]?"="      { 
								yylval.ptrchar = yytext; 
								if (full_info) 
									printf("Variable init at line %i\n", lines_count);
								return ANY_EQ; 
								}
":"                             { yylval.charval = *yytext; return COLON; }
[\[\]]       { }
[ \t]         { yylval.charval = *yytext; }


[\t]*{VAR_REF}      { yylval.ptrchar = yytext; return VAR_REF; }
";"                                  { yylval.charval = *yytext; return ';'; }

"("         { yylval.charval = *yytext; return '('; }
")"         { yylval.charval = *yytext; return ')'; }
"{"         { yylval.charval = *yytext; return '{'; }
"}"         { yylval.charval = *yytext; return '}'; }
"$"         { yylval.charval = *yytext; return '$'; }
"-"         { yylval.charval = *yytext; return '-'; }
"+"         { yylval.charval = *yytext; return '+'; }
"\""        { yylval.charval = *yytext; return '"'; }
"|"         { yylval.charval = *yytext; return '|'; }
"/"         { yylval.charval = *yytext; return '/'; }
"&"         { yylval.charval = *yytext; return '&'; }
"<"         { yylval.charval = *yytext; return '<'; }
","         { yylval.charval = *yytext; return ','; }
"?"         { yylval.charval = *yytext; return '?'; }
"!"         { yylval.charval = *yytext; return '!'; }
">"         { yylval.charval = *yytext; return '>'; }
"`"         { yylval.charval = *yytext; return '`'; }
"@"         { yylval.charval = *yytext; }

".PHONY"                    { yylval.ptrchar = yytext; if (full_info) printf("Special %s at line %i\n", yytext, lines_count); return SPECIAL; }
".SUFFIXES"                 { yylval.ptrchar = yytext; if (full_info) printf("Special %s at line %i\n", yytext, lines_count); return SPECIAL; }
".DEFAULT"                  { yylval.ptrchar = yytext; if (full_info) printf("Special %s at line %i\n", yytext, lines_count); return SPECIAL; }
".PRECIOUS"                 { yylval.ptrchar = yytext; if (full_info) printf("Special %s at line %i\n", yytext, lines_count); return SPECIAL; }
".INTERMEDIATE"             { yylval.ptrchar = yytext; if (full_info) printf("Special %s at line %i\n", yytext, lines_count); return SPECIAL; }
".SECONDARY"                { yylval.ptrchar = yytext; if (full_info) printf("Special %s at line %i\n", yytext, lines_count); return SPECIAL; }
".DELETE_ON_ERROR"          { yylval.ptrchar = yytext; if (full_info) printf("Special %s at line %i\n", yytext, lines_count); return SPECIAL; }
".IGNORE"                   { yylval.ptrchar = yytext; if (full_info) printf("Special %s at line %i\n", yytext, lines_count); return SPECIAL; }
".SILENT"                   { yylval.ptrchar = yytext; if (full_info) printf("Special %s at line %i\n", yytext, lines_count); return SPECIAL; }
".EXPORT_ALL_VARIABLES"     { yylval.ptrchar = yytext; if (full_info) printf("Special %s at line %i\n", yytext, lines_count); return SPECIAL; }
".NOTPARALLEL"              { yylval.ptrchar = yytext; if (full_info) printf("Special %s at line %i\n", yytext, lines_count); return SPECIAL; }


({UNIT_NAME}|{DIGIT})+              { yylval.ptrchar = yytext; return UNIT_NAME; }
\%({UNIT_NAME}|{DIGIT}|[\.])*       { yylval.ptrchar = yytext; return TEMPLATE;}


({UNIT_NAME}|{DIGIT})+([\.]({UNIT_NAME}|{DIGIT})+)+         { yylval.ptrchar = yytext; return FILE_NAME; }
(\/)?(({VAR_REF}|{UNIT_NAME}|{DIGIT}|[\.]|[\.\.])+(\/)?)+   { yylval.ptrchar = yytext; return FILE_PATH; }


"$"[(\{]"abspath ".*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"addsuffix ".*[,].*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"addprefix ".*[,].*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"and ".*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"basename ".*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"call ".*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"dir ".*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"eval ".*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"error ".*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"file ".*[ ].*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"firstword ".*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"findstring ".*[,].*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"foreach ".*[,].*[,].*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"filter ".*[,].*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"filter-out ".*[,].*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"flavour ".*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"info ".*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"if ".*[,].*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"join ".*[,].*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"lastword ".*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"notdir ".*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"or ".*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"origin ".*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"patsubst %".*[,%].*[,].*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"realpath ".*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"sort ".*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"strip ".*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"suffix ".*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"subst ".*[,].*[,].*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"value ".*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"warning ".*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"wordlist ".*[,].*[,].*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"wildcard ".*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"word ".*[,].*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}
"$"[(\{]"words ".*[)\}] { yylval.ptrchar = yytext; check_brackets(yylval.ptrchar); return FUNCTION;}

"\n"      { yylval.charval = *yytext; lines_count++; return EOL; }
<<EOF>>   { static int once = 0; return once++ ? 0 : EOL; }
.         { printf("\n%c\n", *yytext); yyerror("Unknown symbol");}
%%


void check_brackets(char *string)
{
	int ptr = 0;
	int br = 0;
	int _br = 0;
	while (string[ptr])
    {
        if (string[ptr] == '(') br++;
		if (string[ptr] == ')') br--;
		if (string[ptr] == '{') _br++;
		if (string[ptr] == '}') _br--;
        ptr++;
    }
	if (br | _br) yyerror("wrong brackets");

}

void check_slash(char* string)
{
	int ptr = 0;
	while (string[ptr]) ptr++;
	if (string[ptr - 1] == '\\') command_count--; 
}