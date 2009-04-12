%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "battle.h"
#include "grid.h"
#include "errors.h"
#include "scripting.h"

#define YYERROR_VERBOSE

extern char* yytext;
extern int yyget_lineno();
extern int yylex();
extern int yyerror(char*);

extern game_engine_t game;
extern game_parser_t parser;

void *check_cache_size( void *cache, int cache_lines,
                        int *cache_size, int element_size );
grid_node_t *find_material(char *identifier);
grid_node_t *find_node_type(char tile);

int mapflag=0;
char *yyfilename=NULL;
grid_node_t default_grid_node = { ' ', 20, FALSE, TRUE };
%}


%union {
    int val;
    char *str;
    char car;
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
%token TILE COLOR SOLID VISIBLE
%token EXTENDS
%token EQ_ADD EQ_SUB EQ_MUL EQ_DIV
%token <val> B_TRUE B_FALSE B_MAYBE
%token ON_TOUCH ON_INTERACT

%%

input: definition | input definition;
definition:
	  map_definition
	{
            parser.map_number++;
	}
	| entity_definition
	| action_definition
	| material_definition
	| map_rule
	{
            parser.rule_cache_global_lines++;
	}
	;

/* Maps */

map_definition: MAP IDENTIFIER map_blocks;

map_blocks: map_block
	{
            /* Build map.
             * - Check if all tiles correspond to either a rule
             *   in cache or a permanent rule.
             * - Fill the grid with the appropiate data */

#if 0
            new_grid( &game.grid[parser.map_number], LINES-7, COLS ); /* TODO s/COLS/game.game_win->_maxx/, parse after creating game_win */
#else
            new_grid( &game.grid[parser.map_number], parser.str_cache_lines, strlen(parser.str_cache[0]) );
#endif
#if 0
            for (int j=0; j<parser.str_cache_lines; j++)
                printf("%s\n", parser.str_cache[j]);
            for (int j=0; j<parser.rule_cache_lines; j++)
                printf("%c=%s\n", parser.rule_cache[j].tile, parser.rule_cache[j].name);
#endif
            for (int j=0; j<parser.str_cache_lines; j++)
            {
                int line_length = strlen(parser.str_cache[j]);
                for (int i=0; i<line_length; i++)
                {
                    if (parser.str_cache[j][i] == '\0') break;
                    grid_node_t *node_type = find_node_type( parser.str_cache[j][i] );
                    if ( !node_type )
                    {
                        yytext = parser.str_cache[j];
                        yyerror("undefined tile"); /*TODO: get correct line and character*/
                    }

                    int span_x = game.grid[parser.map_number]->width / line_length,
                        span_y = game.grid[parser.map_number]->height / parser.str_cache_lines;
                    game.grid[parser.map_number]->width = span_x * line_length;
                    game.grid[parser.map_number]->height = span_y * parser.str_cache_lines;
                    for (int sj=0; sj<span_y; sj++)
                    {
                        for (int si=0; si<span_x; si++)
                        {
                            memcpy( grid_node(game.grid[parser.map_number], span_y*j+sj, span_x*i+si),
                                    node_type, sizeof(*node_type) );
                        }
                    }
                }
            }
	}
	| map_blocks ',' map_block
	{
#if 0
            for (int j=0; j<parser.str_cache_lines; j++)
                printf("%s\n", parser.str_cache[j]);
            for (int j=0; j<parser.rule_cache_lines; j++)
                printf("%c=%s\n", parser.rule_cache[j].tile, parser.rule_cache[j].name);
#endif
            for (int j=0; j<parser.str_cache_lines; j++)
            {
                for (int i=0; i<strlen(parser.str_cache[j]); i++)
                {
                    grid_node_t *node = malloc( sizeof(*node) );
                    grid_node_t *node_type = find_node_type( parser.str_cache[j][i] );
                    if ( !node_type )
                    {
                        if (parser.str_cache[j][i] = ' ')
                            continue;
                        yytext = parser.str_cache[j];
                        yyerror("undefined tile"); /*TODO: get correct line and character*/
                    }
                    memcpy( node, node_type, sizeof(*node_type) );
                    grid_node(game.grid[parser.map_number], j, i)->above = node;
                }
            }
	}
	;

map_block: '{'
	{
            parser.str_cache_lines = 0;
	}
	map_content '}'
	{
            parser.rule_cache_lines = parser.rule_cache_global_lines;
	}
	map_block_rules
	;
map_block_rules: | WITH '{' map_rules '}';
map_content: map_line | map_content map_line;

map_line: STRING
	{
            /* Populate cache with map lines */
            parser.str_cache = check_cache_size( parser.str_cache,
                                                 parser.str_cache_lines,
                                                 &parser.str_cache_size,
                                                 sizeof(*parser.str_cache) );
            parser.str_cache[parser.str_cache_lines++] = yylval.str;
	}
	;
map_rules: map_rule | map_rule map_rules;
map_rule:
	{
            parser.rule_cache = check_cache_size( parser.rule_cache,
                                                  parser.rule_cache_lines,
                                                  &parser.rule_cache_size,
                                                  sizeof(*parser.rule_cache) );
	}
	MAP_TILE
	{ parser.rule_cache[parser.rule_cache_lines].tile = yylval.car; }
	'=' map_tile_type
	;
	map_tile_type:
	MATERIAL IDENTIFIER
	{
            grid_node_t *node_type = find_material(yylval.str);
            if ( !node_type )
            {
                yyerror("undefined material");
            }

            memcpy( &parser.rule_cache[parser.rule_cache_lines++].node_type,
                    node_type, sizeof(*node_type) );
	}
	| ENTITY IDENTIFIER
	| ACTION action_trigger function_id

function_id: IDENTIFIER '(' expression_list ')'

/* Entities */

entity_definition: ENTITY IDENTIFIER entity_hierarchy '{' entity_content '}'
	;
entity_content: entity_line | entity_content entity_line
	;
entity_line: st_assignment
	| action_trigger '=' function_id
	;
entity_hierarchy: | EXTENDS '(' superclass_list ')';
superclass_list: IDENTIFIER | superclass_list ',' IDENTIFIER;

/* Actions */

action_definition: ACTION function_id '{' action_content '}';
action_content: action_line | action_content action_line;
action_line: statement;
action_trigger: ON_TOUCH | ON_INTERACT;

/* Materials */

material_definition: MATERIAL IDENTIFIER
	{
            parser.material_cache = check_cache_size( parser.material_cache,
                                                      parser.material_cache_lines,
                                                      &parser.material_cache_size,
                                                      sizeof(*parser.material_cache) );
            parser.material_cache[parser.material_cache_lines].name = yylval.str;
            parser.material_cache[parser.material_cache_lines].tile = default_grid_node;
	}
	'{' material_content '}'
	{
            parser.material_cache_lines++;
	};
material_content: material_line | material_content material_line;
material_line:
	  TILE '=' MAP_TILE
	{ parser.material_cache[parser.material_cache_lines].tile.tile = yylval.car; }
	| COLOR '=' color_id
	{ parser.material_cache[parser.material_cache_lines].tile.color = 10/*TODO*/; }
	| SOLID '=' exp_logical
	{ parser.material_cache[parser.material_cache_lines].tile.solid = yylval.val; }
	| VISIBLE '=' exp_logical
	{ parser.material_cache[parser.material_cache_lines].tile.visible = yylval.val; }
	;
color_id: IDENTIFIER;/*TODO*/


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
exp_logical: B_TRUE | B_FALSE | B_MAYBE
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
	| lvalue EQ_SUB exp_int  { $<val>1 = ( $<val>1 - $<val>3 ); }
	| lvalue EQ_MUL exp_int  { $<val>1 = ( $<val>1 * $<val>3 ); }
	| lvalue EQ_DIV exp_int  { $<val>1 = ( $<val>1 / $<val>3 ); }
	;
st_declaration: type IDENTIFIER;
type: "int" | "string" | "bool";

lvalue: IDENTIFIER | COMPOUND_IDENTIFIER;

%%

int yyerror(char *s)
{
    die( "%s:%d:[\"%s\"] %s\n", yyfilename, yyget_lineno(), yytext, s );
    return 0;
}

void *check_cache_size( void *cache, int cache_lines,
                        int *cache_size, int element_size )
{
    if (cache_lines > (*cache_size)-1)
    {
        (*cache_size) *= 2;
        cache = realloc(cache, (*cache_size) * element_size);
    }
    return cache;
}

grid_node_t *find_material(char *identifier)
{
    for (int i=0; i<parser.material_cache_lines; i++)
    {
        if (!strcmp(identifier,parser.material_cache[i].name))
        {
            return &parser.material_cache[i].tile;
        }
    }
    return NULL;
}

grid_node_t *find_node_type(char tile)
{
    for (int i=0; i<parser.rule_cache_lines; i++)
    {
        if (tile == parser.rule_cache[i].tile)
        {
            return &parser.rule_cache[i].node_type;
        }
    }
    return NULL;
}
