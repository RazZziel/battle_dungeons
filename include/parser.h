#ifndef SCRIPTING_H
#define SCRIPTING_H

#include "global.h"
#include "grid.h"
#include "interpreter.h"
#include "symtable.h"


int parse_script(char *filename);

typedef enum {
    RULE_WHATEVER,
    RULE_MATERIAL,
    RULE_ENTITY,
    RULE_ACTION
} item_type_t;

typedef struct {
    item_type_t type;
    char name;
    union {
        grid_node_t material;
        entity_t entity;
        char *action;
    } data;
} map_rule_t; /* TODO:Merge with definition_t */

typedef struct {
    item_type_t type;
    char *name;
    union {
        grid_node_t material;
        entity_t entity;
        char action;
    } data;
} definition_t;

typedef struct {
    int map_number,
        map_level;

    char **str_cache;
    int str_cache_size,
        str_cache_lines;

    map_rule_t *rule_cache,
        current_rule;
    int rule_cache_size,
        rule_cache_lines,
        rule_cache_global_lines;

    definition_t *definition_cache,
        current_definition;
    int definition_cache_size,
        definition_cache_lines;

    symbol_t *symbol_table;
    int symbol_table_size,
        symbol_table_lines;
} game_parser_t;


void init_parser();
void free_parser();


#endif /* SCRIPTING_H */
