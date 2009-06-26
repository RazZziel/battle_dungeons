%code requires {
#include "interpreter.h"
}

%{
#include "global.h"
#include "parser.h"
#include "engine.h"
#include "errors.h"

#define YYERROR_VERBOSE
#define ASSIGN_CHAR_PROPERTY(_field_,_value_) \
    parser.current_definition.data.entity.data.character->_field_ = _value_
#define COPY_IF_SET(_destination_,_source_,_field_ ) \
    if (_source_->data.entity._field_ != 0)          \
        _destination_->data.entity._field_ =         \
            _source_->data.entity._field_;

extern char *yytext;
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
void copy_entity_t(definition_t *destination, definition_t *source);
void copy_object_t(definition_t *destination, definition_t *source);
void copy_character_t(definition_t *destination, definition_t *source);

int mapflag=0;
char *yyfilename=NULL;
grid_node_t default_grid_node = { ' ', 20, FALSE, /*TRUE*/FALSE };

//#define YYSTYPE ast_value_t
%}


%union {
    int     _int;
    char   *_str;
    char    _car;
    ast_t  *_ast;
    ast_t **_ast_list;
}


%type <_int>       color_id color_pair entity_type expr_int expr_bool expr_bool_maybe
%type <_str>       function_decl function_call
%type <_ast>       expression expr2 expr3 expr4 lvalue statement stm_assignment
%type <_ast_list>  expression_seq

%token <_int> INTEGER B_TRUE B_FALSE COLOR_CONSTANT
%token <_str> IDENTIFIER COMPOUND_IDENTIFIER STRING
%token <_car> MAP_TILE
%token <expression_val> B_MAYBE

%token MAP WITH MATERIAL ENTITY ACTION
%token TILE COLOR SOLID VISIBLE NAME HP MP RANGE_SIGHT AGGRESSIVE
%token PC NPC OBJECT
%token STR DEX CON INTL WIZ CHA
%token EXTENDS
%token EQ_ADD EQ_SUB EQ_MUL EQ_DIV
%token ON_TOUCH ON_INTERACT
%token EQ NE LT LE GT GE OR AND NOT

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
        /* Build map.
         * - Check if all tiles correspond to either a rule
         *   in cache or a permanent rule.
         * - Fill the grid with the appropiate data */
	{
            /* Process 1st map layer */

            new_grid( &game.loaded_grids[parser.map_number],
                      parser.str_cache_lines, strlen(parser.str_cache[0]) );
            game.current_grid = game.loaded_grids[parser.map_number];

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
            /* Process map layer above 1st one */

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
            parser.str_cache[parser.str_cache_lines++] = $1;
	}
	;
map_rules: map_rule | map_rule map_rules;
map_rule:
	MAP_TILE
	{
            memset( &parser.current_rule, 0, sizeof(parser.current_rule) );
            parser.current_rule.name = $1;
	}
	'=' map_tile_type
	{
            parser.current_rule.name = $1;
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
            definition_t *definition = find_definition($2, RULE_MATERIAL);
            if ( !definition )
                yyerror("undefined material '%s'", $2);

            parser.current_rule.type = RULE_MATERIAL;
            memcpy( &parser.current_rule.data.material,
                    &definition->data.material,
                    sizeof(definition->data.material) );
	}
	| ENTITY IDENTIFIER
	{
            definition_t *definition = find_definition($2, RULE_ENTITY);
            if ( !definition )
                yyerror("undefined entity '%s'", $2);

            parser.current_rule.type = RULE_ENTITY;
            memcpy( &parser.current_rule.data.entity,
                    &definition->data.entity,
                    sizeof(definition->data.entity) );
	}
	| ACTION action_trigger function_call
	{
            definition_t *definition = find_definition($3, RULE_ACTION);
            if ( !definition )
            {
                yyerror("undefined action '%s'", $3);
            }

            parser.current_rule.type = RULE_ACTION;
            memcpy( &parser.current_rule.data.action,
                    &definition->data.action,
                    sizeof(definition->data.action) );
	}
	;

function_call: IDENTIFIER '(' expression_seq ')';
function_decl: IDENTIFIER '(' identifier_seq ')';

expression_seq:
	                                  //{ $$ = new_expr_list( NULL ); }
	| expression                      //{ $$ = new_expr_list( $1 ); }
	| expression ',' expression_seq   //{ $$ = add_expr_list( NULL, $1 ); }
	;
identifier_seq:
	                                  //{ $$ = new_expr_list( NULL ); }
	| IDENTIFIER                      //{ $$ = new_expr_list( $1 ); }
	| IDENTIFIER ',' identifier_seq   //{ $$ = add_expr_list( NULL, $1 ); }
	;

/* Entities */

entity_definition: ENTITY entity_type IDENTIFIER
	{
            parser.current_definition.name = $3;
            parser.current_definition.type = RULE_ENTITY;

            parser.current_definition.data.entity.type = $2;
            switch( $2 )
            {
            case ENTITY_PC:
            case ENTITY_NPC:
                parser.current_definition.data.entity.data.character = malloc(sizeof(character_t));
                memset(parser.current_definition.data.entity.data.character, 0, sizeof(character_t));
                break;
            case ENTITY_OBJECT:
                parser.current_definition.data.entity.data.object = malloc(sizeof(object_t));
                memset(parser.current_definition.data.entity.data.object, 0, sizeof(object_t));
                break;
            default:
                yyerror( "Unknown entity type %d", $2 );
            }
	}
	entity_hierarchy '{' entity_content '}'
	{
            parser.definition_cache = check_cache_size( parser.definition_cache,
                                                        parser.definition_cache_lines,
                                                        &parser.definition_cache_size,
                                                        sizeof(*parser.definition_cache) );
            parser.definition_cache[parser.definition_cache_lines++] = parser.current_definition;
	}
	;
entity_type: 
	  PC
	{ $$ = ENTITY_PC; }
	| NPC
	{ $$ = ENTITY_NPC; }
	| OBJECT
	{ $$ = ENTITY_OBJECT; }
	|
	{ yyerror( "Expected type of entity (pc|npc|object)"
                   " before entity identifier." ); }
	;


entity_hierarchy: | EXTENDS '(' superclass_list ')';
superclass_list: superclass_id | superclass_list ',' superclass_id;
superclass_id: IDENTIFIER
	{
            definition_t *definition = find_definition($1, RULE_ENTITY);
            if ( !definition )
                yyerror("undefined entity '%s'", $1);

            copy_entity_t( &parser.current_definition, definition );
	}
	;


entity_content: entity_line | entity_content entity_line;
entity_line:
	  TILE '=' MAP_TILE
	{ parser.current_definition.data.entity.tile = $3; }
	| NAME '=' STRING
	{ parser.current_definition.data.entity.name = $3; }
	| COLOR '=' COLOR_CONSTANT /* color_pair */
	{ parser.current_definition.data.entity.color = $3; }

	| HP   '='  expr_int   { ASSIGN_CHAR_PROPERTY(hp_max, $3); }
	| MP   '='  expr_int   { ASSIGN_CHAR_PROPERTY(mp_max, $3); }
	| STR  '='  expr_int   { ASSIGN_CHAR_PROPERTY(str, $3); }
	| CON  '='  expr_int   { ASSIGN_CHAR_PROPERTY(con, $3); }

	| RANGE_SIGHT  '=' expr_int     { ASSIGN_CHAR_PROPERTY(range_sight, $3); }
	| AGGRESSIVE   '=' expr_bool    { ASSIGN_CHAR_PROPERTY(aggressive, $3); }
	| IDENTIFIER   '=' expression
	| ACTION action_trigger function_call
	{
            definition_t *definition = find_definition($3, RULE_ACTION);
            if ( !definition )
                yyerror("undefined action '%s'", $3);
	}
	;

/* Actions */

action_definition: ACTION function_decl
	{
            parser.current_definition.name = $2;
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
            parser.current_definition.name = $2;
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
	{ parser.current_definition.data.material.tile = $3; }
	| COLOR '=' color_pair
	{ parser.current_definition.data.material.color = $3; }
	| SOLID '=' expr_bool
	{ parser.current_definition.data.material.solid = $3; }
	| VISIBLE '=' expr_bool
	{ parser.current_definition.data.material.visible = $3; }
	;
color_pair:
	/* Initialize pair of ncurses colors */
	  color_id
	{
            static int pair_id = 36; /* FIXME:Yes I know it's shit */
            if (pair_id < COLOR_PAIRS-1)
                pair_id++;
            else
                fprintf( stderr, "Too many color pairs\n" );
            init_pair( pair_id, $1, COLOR_BLACK );   
	}
	| color_id ',' color_id
	{
            static int pair_id = 8; /* FIXME:Yes I know it's shit */
            if (pair_id < COLOR_PAIRS-1)
                pair_id++;
            else
                fprintf( stderr, "Too many color pairs\n" );
            init_pair( pair_id, $1, $3 );   
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
            fprintf(stderr,">>(%d,%d,%d)=%d\n",$2, $4, $6, color_id);
            init_color( color_id, $2, $4, $6 );
            $$ = color_id;
#endif
	}
	| COLOR_CONSTANT
	{ $$ = $1; }
	;


/*
 * Expressions
 */


expression:
	  expr2               { $$ = $1; } 
	| expr2   EQ   expr2  { $$ = new_cons_bin( $1, $3, EXPR_EQ ); }
	| expr2   NE   expr2  { $$ = new_cons_bin( $1, $3, EXPR_NE ); }
	| expr2   LT   expr2  { $$ = new_cons_bin( $1, $3, EXPR_LT ); }
	| expr2   LE   expr2  { $$ = new_cons_bin( $1, $3, EXPR_LE ); }
	| expr2   GT   expr2  { $$ = new_cons_bin( $1, $3, EXPR_GT ); }
	| expr2   GE   expr2  { $$ = new_cons_bin( $1, $3, EXPR_GE ); }
	| expr2   OR   expr2  { $$ = new_cons_bin( $1, $3, EXPR_BOOL ); }
	| expr2   AND  expr2  { $$ = new_cons_bin( $1, $3, EXPR_BOOL ); }
	;

expr2:
	  expr3               { $$ = $1; }
	| expr2   '+'  expr3  { $$ = new_cons_bin( $1, $3, EXPR_ADD ); }
	| expr2   '-'  expr3  { $$ = new_cons_bin( $1, $3, EXPR_SUB ); }
	;

expr3:
	  expr4               { $$ = $1; }
	| expr3   '*'  expr4  { $$ = new_cons_bin( $1, $3, EXPR_MUL ); }
	| expr3   '/'  expr4  { $$ = new_cons_bin( $1, $3, EXPR_DIV ); }
	;

expr4:
	  NOT expr4              { $$ = new_cons_uni( $2, EXPR_NOT ); }
	| '(' expression ')'     { $$ = $2; }
	| expr_int               { $$ = new_value( (ast_value_t) $1, EXPR_INT ); }
	| IDENTIFIER             { $$ = new_value( (ast_value_t) $1, EXPR_VAR ); }
	| STRING                 { $$ = new_value( (ast_value_t) $1, EXPR_STR ); }
        | expr_bool              { $$ = new_value( (ast_value_t) $1, EXPR_BOOL ); }
	| expr_bool_maybe        { $$ = new_value( (ast_value_t) $1, EXPR_BOOL_MAYBE ); }
        //| function_call          { $$ = new_fcall( (ast_value_t) $1, EXPR_FCALL ); }
        //| '(' expression_seq ')' { $$ = new_value( (ast_value_t) $2, EXPR_LIST ); }
	;
expr_int: INTEGER;
expr_bool: B_TRUE | B_FALSE;
expr_bool_maybe:
	B_MAYBE
	{
            $$ = 50;
	}
	| B_MAYBE '(' INTEGER ')' // Turn INTEGER into expr_int and eval()?
	{
            if (($3 < 0) || ($3 > 100))
                yyerror( "probability values must be between 0 and 100" );
            $$ = $3;
	}
	;


/*
 * Statements
 */


statement:
	  stm_assignment
	| function_call;
stm_assignment:
	  lvalue  '='     expression  { $$ = new_assig( $1, $3 ); }
	| lvalue  EQ_ADD  expression  { $$ = new_assig( $1, new_cons_bin( $1, $3, EXPR_ADD ) ); }
	| lvalue  EQ_SUB  expression  { $$ = new_assig( $1, new_cons_bin( $1, $3, EXPR_SUB ) ); }
	| lvalue  EQ_MUL  expression  { $$ = new_assig( $1, new_cons_bin( $1, $3, EXPR_MUL ) ); }
	| lvalue  EQ_DIV  expression  { $$ = new_assig( $1, new_cons_bin( $1, $3, EXPR_DIV ) ); }
	;

lvalue:
          IDENTIFIER           { $$ = new_value( (ast_value_t) $1, EXPR_VAR ); }
	| COMPOUND_IDENTIFIER  { $$ = new_value( (ast_value_t) $1, EXPR_VAR ); }
	;


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
        break;

    case RULE_ENTITY:
        rule->data.entity.x = i;
        rule->data.entity.y = j;

        game.entities = realloc( game.entities, sizeof(entity_t*) * (game.n_entities+1) );
        game.entities[game.n_entities] = malloc( sizeof(entity_t) );
        memcpy( game.entities[game.n_entities],
                &rule->data.entity,
                sizeof(entity_t) );

        switch( rule->data.entity.type )
        {
        case ENTITY_PC:
            game.pc = game.entities[game.n_entities];
            break;
        case ENTITY_NPC:
            game.npcs = realloc( game.npcs, sizeof(*game.npcs) * (game.n_npcs+1) );
            game.npcs[game.n_npcs] = game.entities[game.n_entities];
            game.npcs[game.n_npcs]->data.character = malloc( sizeof(character_t) );
            memcpy( game.npcs[game.n_npcs]->data.character,
                    rule->data.entity.data.character,
                    sizeof(character_t) );
            game.n_npcs++;
            break;
        case ENTITY_OBJECT:
            break;
        default:
            yyerror( "Unknown entity type %d", rule->data.entity.type );
        }

        game.n_entities++;
        break;

    case RULE_ACTION:
        break;

    default:
        yyerror( "Unknown rule type %d", rule->type );
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

/*TODO: Pig disgusting. Refactor into some sort of class methods. Now.*/
void copy_entity_t(definition_t *destination, definition_t *source)
{
    /* Copy fields that are set to some value to the current
       definition being parsed, as a default value that can
       be later overwritten */

    COPY_IF_SET(destination, source, x);
    COPY_IF_SET(destination, source, y);
    COPY_IF_SET(destination, source, color);
    COPY_IF_SET(destination, source, tile);
    COPY_IF_SET(destination, source, name);

    switch( source->data.entity.type )
    {
    case ENTITY_PC:
    case ENTITY_NPC:
        copy_character_t( destination, source );
        break;
    case ENTITY_OBJECT:
        copy_object_t( destination, source );
        break;
    default:
        break;
    }
}

void copy_object_t(definition_t *destination, definition_t *source)
{
    COPY_IF_SET(destination, source, data.object->price);
}
void copy_character_t(definition_t *destination, definition_t *source)
{
    COPY_IF_SET(destination, source, data.character->hp);
    COPY_IF_SET(destination, source, data.character->mp);
    COPY_IF_SET(destination, source, data.character->gender);
    COPY_IF_SET(destination, source, data.character->race);
    COPY_IF_SET(destination, source, data.character->class_);
    COPY_IF_SET(destination, source, data.character->alignment1);
    COPY_IF_SET(destination, source, data.character->alignment2);

    COPY_IF_SET(destination, source, data.character->deity);
    COPY_IF_SET(destination, source, data.character->age);
    COPY_IF_SET(destination, source, data.character->height);
    COPY_IF_SET(destination, source, data.character->weight);

    COPY_IF_SET(destination, source, data.character->str);
    COPY_IF_SET(destination, source, data.character->dex);
    COPY_IF_SET(destination, source, data.character->con);
    COPY_IF_SET(destination, source, data.character->intl);
    COPY_IF_SET(destination, source, data.character->wiz);
    COPY_IF_SET(destination, source, data.character->cha);

    COPY_IF_SET(destination, source, data.character->hp);
    COPY_IF_SET(destination, source, data.character->hp_max);
    COPY_IF_SET(destination, source, data.character->mp);
    COPY_IF_SET(destination, source, data.character->mp_max);
    COPY_IF_SET(destination, source, data.character->size);
    COPY_IF_SET(destination, source, data.character->speed);

    COPY_IF_SET(destination, source, data.character->exp);
    COPY_IF_SET(destination, source, data.character->level);
    COPY_IF_SET(destination, source, data.character->range_sight);
    COPY_IF_SET(destination, source, data.character->aggressive);
}
