%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "battle.h"
#include "grid.h"
#include "errors.h"
#include "scripting.h"

#define YYERROR_VERBOSE

extern char* yytext;
extern int yyget_lineno();
extern int yylex();
extern int yyerror(const char *fmt, ...);

extern game_engine_t game;
extern game_parser_t parser;

void *check_cache_size( void *cache, int cache_lines,
                        int *cache_size, int element_size );
definition_t *find_definition(char *name, item_type_t type);
map_rule_t *find_rule(char name, item_type_t type);
void assign_node(grid_node_t *node, char *line, int j, int i);

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

%type <val> exp_int exp_logical color_id

%token <val> INTEGER
%token <str> STRING
%token <str> IDENTIFIER
%token COMPOUND_IDENTIFIER
%token MAP_TILE COLOR_CONSTANT

%token MAP WITH MATERIAL ENTITY ACTION
%token TILE COLOR SOLID VISIBLE NAME HP MP RANGE_SIGHT
%token EXTENDS
%token EQ_ADD EQ_SUB EQ_MUL EQ_DIV
%token <val> B_TRUE B_FALSE B_MAYBE
%token ON_TOUCH ON_INTERACT

%%

input: definition | input definition;
definition:
	{
	    memset( &parser.current_definition, 0,
	            sizeof(parser.current_definition) );
	}
	  _definition
	| _rule
	{
            parser.rule_cache_global_lines++;
	}
	;
_definition:
	  map_definition
	{ parser.map_number++; }
	| entity_definition
	| action_definition
	| material_definition
	;
_rule: map_rule;

/* Maps */

map_definition: MAP IDENTIFIER map_blocks;

map_blocks: map_block
	{
            /* Build map.
             * - Check if all tiles correspond to either a rule
             *   in cache or a permanent rule.
             * - Fill the grid with the appropiate data */

            new_grid( &game.loaded_grids[parser.map_number],
                      parser.str_cache_lines, strlen(parser.str_cache[0]) );
            game.current_grid = game.loaded_grids[parser.map_number];
#if 0
            for (int j=0; j<parser.str_cache_lines; j++)
                fprintf(stderr,"%s\n", parser.str_cache[j]);
            for (int j=0; j<parser.rule_cache_lines; j++)
                fprintf(stderr,"%c=%c\n", parser.rule_cache[j].name,
                        parser.rule_cache[j].data.material.tile);
#endif
            for (int j=0; j<parser.str_cache_lines; j++)
            {
                for (int i=0; i<strlen(parser.str_cache[j]); i++)
                {
                    assign_node( grid_node(game.current_grid, j, i),
                                 parser.str_cache[j], j, i );
                }
            }
	}
	| map_blocks ',' map_block
	{
#if 0
            for (int j=0; j<parser.str_cache_lines; j++)
                fprintf(stderr,"%s\n", parser.str_cache[j]);
            for (int j=0; j<parser.rule_cache_lines; j++)
                fprintf(stderr,"%c=%c\n", parser.rule_cache[j].name,
                        parser.rule_cache[j].data.material.tile);
#endif
            for (int j=0; j<parser.str_cache_lines; j++)
            {
                for (int i=0; i<strlen(parser.str_cache[j]); i++)
                {
                    if ( parser.str_cache[j][i] != ' ' )
                    {
                        assign_node( NULL,
                                     parser.str_cache[j], j, i );
                    }
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
            parser.str_cache[parser.str_cache_lines++] = $<str>1;
	}
	;
map_rules: map_rule | map_rule map_rules;
map_rule:
	MAP_TILE
	{
            memset( &parser.current_rule, 0, sizeof(parser.current_rule) );
            parser.current_rule.name = $<car>1;
	}
	'=' map_tile_type
	{
            parser.current_rule.name = $<car>1;
            parser.rule_cache = check_cache_size( parser.rule_cache,
                                                  parser.rule_cache_lines,
                                                  &parser.rule_cache_size,
                                                  sizeof(*parser.rule_cache) );
            parser.rule_cache[parser.rule_cache_lines++] = parser.current_rule;
	}
	;
map_tile_type:
	MATERIAL IDENTIFIER
	{
            definition_t *definition = find_definition($<str>2, RULE_MATERIAL);
            if ( !definition )
                yyerror("undefined material '%s'", $<str>2);

            parser.current_rule.type = RULE_MATERIAL;
            memcpy( &parser.current_rule.data.material,
                    &definition->data.material,
                    sizeof(definition->data.material) );
	}
	| ENTITY IDENTIFIER
	{
            definition_t *definition = find_definition($<str>2, RULE_ENTITY);
            if ( !definition )
                yyerror("undefined entity '%s'", $<str>2);

            parser.current_rule.type = RULE_ENTITY;
            memcpy( &parser.current_rule.data.entity,
                    &definition->data.entity,
                    sizeof(definition->data.entity) );
	}
	| ACTION action_trigger function_id
	{
            definition_t *definition = find_definition($<str>3, RULE_ACTION);
            if ( !definition )
            {
                yyerror("undefined action '%s'", $<str>3);
            }

            parser.current_rule.type = RULE_ACTION;
            memcpy( &parser.current_rule.data.action,
                    &definition->data.action,
                    sizeof(definition->data.action) );
	}

function_id: IDENTIFIER '(' expression_list ')'

/* Entities */

entity_definition: ENTITY IDENTIFIER entity_hierarchy
	{
            parser.current_definition.name = $<str>2;
            parser.current_definition.type = RULE_ENTITY;
	}
	'{' entity_content '}'
	{
            if (!strcmp(parser.current_definition.name, "player")) /* FIXME:lol */
            {
                game.pc = parser.current_definition.data.entity;
            }
            parser.definition_cache = check_cache_size( parser.definition_cache,
                                                        parser.definition_cache_lines,
                                                        &parser.definition_cache_size,
                                                        sizeof(*parser.definition_cache) );
            parser.definition_cache[parser.definition_cache_lines++] = parser.current_definition;
	}
	;
entity_hierarchy: | EXTENDS '(' superclass_list ')';
superclass_list: IDENTIFIER | superclass_list ',' IDENTIFIER;
entity_content: entity_line | entity_content entity_line;
entity_line:
	  TILE '=' MAP_TILE
	{ parser.current_definition.data.entity.tile = $<car>3; }
	| NAME '=' STRING
	{ parser.current_definition.data.entity.name = $<str>3; }
	| COLOR '=' COLOR_CONSTANT /* color_pair */
	{ parser.current_definition.data.entity.color = $<val>3; }
	| HP '=' INTEGER
	{ parser.current_definition.data.entity.hp_max = $<val>3; }
	| MP '=' INTEGER
	{ parser.current_definition.data.entity.mp_max = $<val>3; }
	| RANGE_SIGHT '=' INTEGER
	{ parser.current_definition.data.entity.range_sight = $<val>3; }
	| IDENTIFIER '=' expression
	| COMPOUND_IDENTIFIER '=' expression
	| ACTION action_trigger function_id
	{
            definition_t *definition = find_definition($<str>3, RULE_ACTION);
            if ( !definition )
                yyerror("undefined action '%s'", $<str>3);
	}
	;

/* Actions */

action_definition: ACTION function_id
	{
            parser.current_definition.name = $<str>2;
            parser.current_definition.type = RULE_ACTION;
	}
	'{' action_content '}'
	{
            parser.definition_cache = check_cache_size( parser.definition_cache,
                                                        parser.definition_cache_lines,
                                                        &parser.definition_cache_size,
                                                        sizeof(*parser.definition_cache) );
            parser.definition_cache[parser.definition_cache_lines++] = parser.current_definition;
	}
	;
action_content: action_line | action_content action_line;
action_line: statement;
action_trigger: ON_TOUCH | ON_INTERACT;

/* Materials */

material_definition: MATERIAL IDENTIFIER
	{
            parser.current_definition.name = $<str>2;
            parser.current_definition.type = RULE_MATERIAL;
            parser.current_definition.data.material = default_grid_node;
	}
	'{' material_content '}'
	{
            parser.definition_cache = check_cache_size( parser.definition_cache,
                                                        parser.definition_cache_lines,
                                                        &parser.definition_cache_size,
                                                        sizeof(*parser.definition_cache) );
            parser.definition_cache[parser.definition_cache_lines++] = parser.current_definition;
	};
material_content: material_line | material_content material_line;
material_line:
	  TILE '=' MAP_TILE
	{ parser.current_definition.data.material.tile = $<car>3; }
	| COLOR '=' color_pair
	{
            parser.current_definition.data.material.color = $<val>3;
	}
	| SOLID '=' exp_logical
	{ parser.current_definition.data.material.solid = $<val>3; }
	| VISIBLE '=' exp_logical
	{ parser.current_definition.data.material.visible = $<val>3; }
	;
color_pair:
	/* Initialize pair of ncurses colors */
	  color_id
	{
            static int pair_id = 38; /* FIXME:Yes I know it's shit */
            if (pair_id < COLOR_PAIRS-1)
                pair_id++;
            else
                fprintf( stderr, "Too many color pairs\n" );
            init_pair( pair_id, $<val>1, COLOR_BLACK );   
	}
	| color_id ',' color_id
	{
            static int pair_id = 8; /* FIXME:Yes I know it's shit */
            if (pair_id < COLOR_PAIRS-1)
                pair_id++;
            else
                fprintf( stderr, "Too many color pairs\n" );
            init_pair( pair_id, $<val>1, $<val>3 );   
	}
	;
color_id:
	/* Initialize ncurses color */
	{ $$ = COLOR_BLACK; }
	| '(' INTEGER ',' INTEGER ',' INTEGER ')'
	{
#if 0 /* ncurses sucks big time */
            static int color_id = 0;
            if (color_id < COLORS)
                color_id++;
            else
                fprintf( stderr, "Too many colors\n" );
            fprintf(stderr,">>(%d,%d,%d)=%d\n",$<val>2, $<val>4, $<val>6, color_id);
            init_color( color_id, $<val>2, $<val>4, $<val>6 );
            $$ = color_id;
#endif
	}
	| COLOR_CONSTANT
	{ $$ = $<val>1; }
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

int yyerror(const char *fmt, ...)
{
    char errstr[512] = "";
    va_list argp;
    va_start(argp, fmt);

    snprintf(errstr, 512, "%s:%d %s\n",
             yyfilename, yyget_lineno(), fmt);
    vdie(errstr, argp);

    va_end(argp);
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

void assign_node(grid_node_t *node, char *line, int j, int i)
{
    if ( line[i] == ' ' )/*FIXME PIG DISGUSTING*/
    {
        (*node) = default_grid_node;
        return;
    }

    map_rule_t *rule = find_rule( line[i], RULE_WHATEVER );
    if ( !rule )
        yyerror("undefined rule '%c'", line[i]);

    switch( rule->type )
    {
    case RULE_MATERIAL:
        if (!node)
        {
            /* Node above base grid (level>0): find the end of
               the node list for grid[j,i] allocate new memory
               and copy. */
            grid_node_t *iter;
            for ( iter = grid_node(game.current_grid, j, i);
                  iter->above != NULL;
                  iter = iter->above );
            iter->above = malloc( sizeof(grid_node_t) );
            node = iter->above;
        }
        (*node) = (rule->data.material);

    case RULE_ENTITY:
        if ( line[i] == '@' )/*FIXME:pig disgusting*/
        {
            game.pc.x = i;
            game.pc.y = j;
            return;
        }
        break;

    case RULE_ACTION:
        break;

    default: break;
    }
}

/* When we are reading a map rule,
   find the appropiate definition */
definition_t *find_definition(char *name, item_type_t type)
{
    for (int i=0; i<parser.definition_cache_lines; i++)/*TODO:{b,h}search*/
    {
        if ( (!strcmp(name, parser.definition_cache[i].name)) &&
             (type == parser.definition_cache[i].type) )
        {
            return &parser.definition_cache[i];
        }
    }
    return NULL;
}

/* When we are reading a map,
   find the appropiate map rule */
map_rule_t *find_rule(char name, item_type_t type)
{
    for (int i=0; i<parser.rule_cache_lines; i++)/*TODO:{b,h}search*/
    {
        if ( (name == parser.rule_cache[i].name) &&
             ( (type == parser.rule_cache[i].type) ||
               (type == RULE_WHATEVER) ) )
        {
            return &parser.rule_cache[i];
        }
    }
    return NULL;
}
