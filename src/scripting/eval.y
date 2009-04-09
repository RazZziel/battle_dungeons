%{
#include <stdlib.h>
#include <stdio.h>

#define YYERROR_VERBOSE
int mapflag=0;
%}

%union {
    int val;
    char *str;
}

%expect 17

%type <val> exp_logical

%token <val> INTEGER
%token <str> STRING
%token <str> IDENTIFIER
%token COMPOUND_IDENTIFIER
%token MAP_CONTENT
%token MAP_TILE

%token MAP WITH MATERIAL ENTITY ACTION
%token <val> TRUE FALSE MAYBE
%token ON_TOUCH ON_INTERACT

%%

input: definition input | definition;
definition: map_definition | entity_definition | action_definition;

/* Maps */

map_definition: MAP IDENTIFIER map_blocks;
map_blocks: map_block ',' map_blocks | map_block;
map_block: '{' map_content '}' map_block_rules;
map_block_rules: | WITH '{' map_rules '}';
map_content: MAP_CONTENT;
map_rules: map_rule map_rules | map_rule;
map_rule: MAP_TILE '=' map_tile_type;
map_tile_type: MATERIAL IDENTIFIER
	| ENTITY IDENTIFIER
	| ACTION action_trigger function_id

function_id: IDENTIFIER '(' expression_list ')'

/* Entities */

entity_definition: ENTITY IDENTIFIER '{' entity_content '}'
	;
entity_content: entity_line entity_content | entity_line
	;
entity_line: st_assignment
	;

/* Actions */

action_definition: ACTION function_id '{' action_content '}'
	;
action_content: action_line action_content | action_line
	;
action_line: statement
	;
action_trigger: ON_TOUCH | ON_INTERACT;


expression: IDENTIFIER
	| COMPOUND_IDENTIFIER
	| exp_logical
	| exp_int
	| exp_string
	| exp_tuple
	| exp_inventory;
	| '(' expression ')'
	;
exp_logical: TRUE | FALSE | MAYBE
	| exp_logical "||" exp_logical  { $$ = ( $1 || $3 ); }
	| exp_logical "&&" exp_logical  { $$ = ( $1 && $3 ); }
	| exp_logical "==" exp_logical  { $$ = ( $1 == $3 ); }
	| exp_logical "!=" exp_logical  { $$ = ( $1 != $3 ); }
	;
exp_int:       INTEGER;
exp_string:    STRING;
exp_tuple:     '(' expression_list ')';
exp_inventory: INTEGER '*' IDENTIFIER;
expression_list: | expression ',' expression_list | expression;


statement: st_assignment | st_declaration | IDENTIFIER expression;
st_assignment: lvalue '=' expression
	;
st_declaration: type IDENTIFIER;
type: "int" | "string" | "bool";

lvalue: IDENTIFIER | COMPOUND_IDENTIFIER;

%%

int yyerror(char *s)
{
    fprintf( stderr, "[%s]\n", s );
    return 0;
}

int main(int argc, char *argv[])
{
    yyparse();	
}

