%{
/* This first section contains C code which will be included in the output
   file.
*/
#include <stdlib.h>
#include <stdio.h>
/* Since we are using C++, we need to specify the prototypes for some 
   internal yacc functions so that they can be found at link time.
*/
extern int yylex(void);
extern void yyerror(char *msg);
%}

/* This is a union of the different types of values that a token can
   take on.  In our case we'll just handle "numbers", which are of
   C int type.
*/
%union {
	int number;
}

/* These are untyped tokens which are recognized as part of the grammar */
%token AND OR EQUALS

/* Here we are, any NUMBER token is stored in the number member of the
   union above.
*/
%token  NUMBER

/* These rules all return a numeric value */
%type 
 expression
%type 
 logical_expression and or equals
%%

/* Our language consists either of a single statement or of a list of statements.
   Notice the recursivity of the rule, this allows us to have any
   number of statements in a statement list.
*/
statement_list: statement | statement_list statement
	;
	
/* A statement is simply an expression.  When the parser sees an expression
   we print out its value for debugging purposes.  Later on we'll
   have more than just expressions in our statements.
*/
statement: expression
	{ printf("Expression = %d\n", $1); }
	;
	
/* An expression can be a number or a logical expression. */
expression: NUMBER
	|   logical_expression
	;
	
/* We have a few different types of logical expressions */
logical_expression: and
	|           or
	|           equals
	;
	
/* When the parser sees two expressions surrounded by parenthesis and
   connected by the AND token, it will actually perform a C logical
   expression and store the result into $$, which is the "value" of
   this statement.
*/
and: '(' expression AND expression ')'
	{ if ( $2 && $4 ) { $$ = 1; } else { $$ = 0; } }
	;
	
or: '(' expression OR expression ')'
	{ if ( $2 || $4 ) { $$ = 1; } else { $$ = 0; } }
	;
	
equals: '(' expression EQUALS expression ')'
	{ if ( $2 == $4 ) { $$ = 1; } else { $$ = 0; } }
	;
	
%%

/* This is a sample main() function that just parses standard input
   using our yacc grammar.  It allows us to feed sample scripts in
   and see if they are parsed correctly.
*/
int main(int argc, char *argv[])
{
	yyparse();
	
}
/* This is an error function used by yacc, and must be defined */-
void yyerror(char *message)
{
	fprintf(stderr, "%s\n", message);
}

