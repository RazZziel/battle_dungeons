%{
#include <stdlib.h>
#include <stdio.h>

#define YYERROR_VERBOSE
int mapflag=0;
char *yyfilename=NULL;
extern char* yytext;
%}

%union {
    int val;
    char *str;
}

%expect 34

%type <val> exp_int
%type <val> exp_logical

%token <val> INTEGER
%token <str> STRING
%token <str> IDENTIFIER
%token COMPOUND_IDENTIFIER
%token MAP_TILE

%token MAP WITH MATERIAL ENTITY ACTION
%token EXTENDS
%token EQ_ADD EQ_SUB EQ_MUL EQ_DIV
%token <val> TRUE FALSE MAYBE
%token ON_TOUCH ON_INTERACT

%%

input: definition input | definition;
definition: map_definition | entity_definition | action_definition | material_definition;

/* Maps */

map_definition: MAP IDENTIFIER map_blocks;
map_blocks: map_block ',' map_blocks | map_block;
map_block: '{' map_content '}' map_block_rules;
map_block_rules: | WITH '{' map_rules '}';
map_content: STRING map_content | STRING;
map_rules: map_rule map_rules | map_rule;
map_rule: MAP_TILE '=' map_tile_type;
map_tile_type: MATERIAL IDENTIFIER
	| ENTITY IDENTIFIER
	| ACTION action_trigger function_id

function_id: IDENTIFIER '(' expression_list ')'

/* Entities */

entity_definition: ENTITY IDENTIFIER entity_hierarchy '{' entity_content '}'
	;
entity_content: entity_line entity_content | entity_line
	;
entity_line: st_assignment
	| action_trigger '=' function_id
	;
entity_hierarchy: | EXTENDS '(' superclass_list ')'
	;
superclass_list: IDENTIFIER ',' superclass_list | IDENTIFIER
	;

/* Actions */

action_definition: ACTION function_id '{' action_content '}'
	;
action_content: action_line action_content | action_line
	;
action_line: statement
	;
action_trigger: ON_TOUCH | ON_INTERACT;

/* Materials */

material_definition: MATERIAL IDENTIFIER '{' material_content '}'
	;
material_content: material_line material_content | material_line
	;
material_line: st_assignment
	;


expression: IDENTIFIER
	| COMPOUND_IDENTIFIER
	| exp_logical
	| exp_int
	| exp_string
	| exp_tuple
	| exp_inventory
	| '(' expression ')'
	| MAP_TILE
	;
exp_logical: TRUE | FALSE | MAYBE
	| exp_logical "||" exp_logical  { $$ = ( $1 || $3 ); }
	| exp_logical "&&" exp_logical  { $$ = ( $1 && $3 ); }
	| exp_logical "==" exp_logical  { $$ = ( $1 == $3 ); }
	| exp_logical "!=" exp_logical  { $$ = ( $1 != $3 ); }
	;
exp_int: INTEGER
	| exp_int '+' exp_int  { $$ = ( $1 + $3 ); }
	| exp_int '-' exp_int  { $$ = ( $1 - $3 ); }
	| exp_int '*' exp_int  { $$ = ( $1 * $3 ); }
	| exp_int '/' exp_int  { $$ = ( $1 / $3 ); }
	;
exp_string: STRING;
exp_tuple: '(' expression_list ')';
exp_inventory: INTEGER '*' IDENTIFIER;
expression_list: | expression ',' expression_list | expression;


statement: st_assignment | st_declaration | function_id;
st_assignment: lvalue '=' expression
	| lvalue EQ_ADD exp_int  { $<val>1 = ( $<val>1 + $<val>3 ); }
	| lvalue EQ_SUB  exp_int  { $<val>1 = ( $<val>1 - $<val>3 ); }
	| lvalue EQ_MUL exp_int  { $<val>1 = ( $<val>1 * $<val>3 ); }
	| lvalue EQ_DIV  exp_int  { $<val>1 = ( $<val>1 / $<val>3 ); }
	;
st_declaration: type IDENTIFIER;
type: "int" | "string" | "bool";

lvalue: IDENTIFIER | COMPOUND_IDENTIFIER;

%%

int yyerror(char *s)
{
    fprintf( stderr, "%s:%d:[\"%s\"] %s\n", yyfilename, yyget_lineno(), yytext, s );
    return 0;
}
