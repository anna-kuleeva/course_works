%{
	#include <stdlib.h>
	#include <stdio.h>
	int lines_count = 1;
	extern int full_info;
	int yylex(void);
	void yyerror(const char *s) 
	{
        fprintf (stderr, "\n!ERROR! line %i: %s\n", lines_count, s);
		exit(0);
    }

	
	int init_count = 0;
	int target_count = 0;
	int cond_count = 0;
	int in_target = 0;
	int comments_count = 0;
	int command_count = 0;
    		
%}

%union
{
	int 	intval; 
	char 	charval;
	char*	ptrchar;
};


%type <intval> DIGIT cond
%type <charval> LETTER UPLETTER COLON ANY_EQ TAB
%type <ptrchar> IFNEQ IFEQ ELSE IFDEF IFNDEF ENDEF ENDIF  
%type <ptrchar> COMMAND UNIT_NAME FILE_NAME RUN_COMMAND ANY_STRING
%type <ptrchar> FILE_PATH COLEQ VAR_REF TARGET_SLASH TEMPLATE
%type <ptrchar> FUNCTION SPECIAL SHELL_COMMAND target_name
 
%token IFNEQ IFEQ ELSE IFDEF IFNDEF ENDEF ENDIF TAB
%token DIGIT LETTER UPLETTER COLON EOL UNIT_NAME RUN_COMMAND COMMAND
%token FILE_NAME ANY_EQ DOLLAR OBR CBR COLEQ FILE_PATH VAR_REF
%token INCLUDE DEFINE EXPORT SHELL_COMMAND
%token TARGET_SLASH ANY_STRING TEMPLATE FUNCTION SPECIAL



%%
input: line
	| input line
	;

line: EOL
	| target			{ target_count++; if (full_info) printf("Commands: %i\n", command_count); command_count = 0; in_target = 0; }
	| variable_init		{ init_count++; }
	| condition EOL		{ cond_count++; }
	| FUNCTION EOL		
	| INCLUDE spec_incs EOL
	| SPECIAL COLON spec_incs EOL
	| SPECIAL COLON EOL
	| DEFINE variable_init ENDEF EOL
	; 

target: target_names COLON EOL recipes EOL
	| target_names COLON prerequisites EOL recipes EOL
	| target_names COLON prerequisites EOL
	| target_names COLON ';' EOL
	;

variable_init: 
	 UNIT_NAME ANY_EQ prerequisites EOL
	| UNIT_NAME ANY_EQ EOL
	| UNIT_NAME ANY_EQ FUNCTION EOL
	| UNIT_NAME ANY_EQ SHELL_COMMAND EOL
	;


prerequisites:
			prerequisite
			| prerequisites prerequisite
			;

prerequisite:
			target_name
			| ANY_STRING
			| SHELL_COMMAND
			| COLON
			| TARGET_SLASH
			| UNIT_NAME ANY_EQ target_name
			;

recipes: 
	  recipe
	| recipes EOL recipe
	| recipes TARGET_SLASH recipe
	;

recipe: 
	RUN_COMMAND 	{ command_count++; }
	| COMMAND 		{ command_count++; }
	| condition		{ cond_count++; }
	;

target_names: 
			target_name		 { if (full_info) printf("\nTarget at line %i: %s\n", lines_count, $1); in_target = 1; }
			| target_names target_name
			| target_names TARGET_SLASH target_name
			;
target_name: 
			VAR_REF				{ $$ = $1; }
			| UNIT_NAME			{ $$ = $1; }
			| FILE_PATH			{ $$ = $1; }
			| FILE_NAME			{ $$ = $1; }
			| TEMPLATE			{ $$ = $1; }
			| '('TEMPLATE')'	{ $$ = $2; }
			;

/* обработка условий */
condition: 
      if cond_construction EOL post_condition
    | ifdef cond EOL ENDIF 
	| ifdef cond EOL ELSE EOL ENDIF
	| ifdef cond EOL variable_inits ENDIF
	| ifdef cond EOL ELSE EOL variable_inits ENDIF
	| ifdef cond EOL variable_inits ELSE EOL ENDIF
	| ifdef cond EOL variable_inits ELSE EOL variable_inits ENDIF
    ;

post_condition:
		  ENDIF
		| elses ENDIF
		| elses ELSE EOL ENDIF
		| elses ELSE EOL variable_inits ENDIF
		| ELSE EOL ENDIF
		| ELSE EOL variable_inits ENDIF
		| variable_inits ENDIF
		| variable_inits elses ENDIF
		| variable_inits elses ELSE EOL ENDIF
		| variable_inits elses ELSE EOL variable_inits ENDIF
		| variable_inits ELSE EOL variable_inits ENDIF
		| variable_inits ELSE EOL ENDIF
;

variable_inits:
	variable_init
	| COMMAND EOL		{ if (!in_target) yyerror("command out of target"); command_count++; }
	| variable_inits variable_init
	| variable_inits COMMAND EOL { if (!in_target) yyerror("command out of target"); command_count++; }
;
elses: 
	ELSE cond_construction EOL variable_inits
	| ELSE cond_construction EOL
	| elses ELSE cond_construction EOL variable_inits
	| elses ELSE cond_construction EOL
;
cond_construction:
			'(' cond ',' cond ')'      {if (full_info) printf("Condition at line %i\n", lines_count); }
			| '(' ',' cond ')'        {if (full_info) printf("Condition at line %i\n", lines_count); }
			| '(' cond ',' ')'        {if (full_info) printf("Condition at line %i\n", lines_count); }
			| '(' ',' ')'            {if (full_info) printf("Condition at line %i\n", lines_count); }
			| ANY_STRING ANY_STRING  {if (full_info) printf("Condition at line %i\n", lines_count); }
			;

cond:
	 UNIT_NAME    {  }
	| FILE_PATH   { }
	| FILE_NAME   { }
	| VAR_REF     { }
	| ANY_STRING  { }
	| FUNCTION    { }
	;

if: 
      IFEQ	
    | IFNEQ	
    ;

ifdef: 
      IFDEF	
    | IFNDEF	
    ;

spec_incs:
	target_name
	| spec_incs target_name
	;

%%

