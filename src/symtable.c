#include "symtable.h"
#include "parser.h"

#define INITIAL_CACHE_SIZE 2

extern game_parser_t parser;

void init_symbol_tables()
{
    parser.str_cache_size = INITIAL_CACHE_SIZE;
    parser.str_cache = malloc(parser.str_cache_size * sizeof(*parser.str_cache));

    parser.rule_cache_size = INITIAL_CACHE_SIZE;
    parser.rule_cache = malloc(parser.rule_cache_size * sizeof(*parser.rule_cache));

    parser.definition_cache_size = INITIAL_CACHE_SIZE;
    parser.definition_cache = malloc(parser.definition_cache_size * sizeof(*parser.definition_cache));

    parser.symbol_table_size = INITIAL_CACHE_SIZE;
    parser.symbol_table = malloc(parser.symbol_table_size * sizeof(*parser.symbol_table));
}

void free_symbol_tables()
{
#if 0
    printf("-------\n");
    for(int i=0;i<parser.rule_cache_lines;i++)
    {
        printf("%c=(%c,%d,%d,%d)\n",parser.rule_cache[i].tile,
               parser.rule_cache[i].node_type.tile, parser.rule_cache[i].node_type.color,
               parser.rule_cache[i].node_type.solid, parser.rule_cache[i].node_type.visible);
    };
    printf("-------\n");
    for(int i=0;i<parser.definition_cache_lines;i++)
    {
        printf("%s=(%c,%d,%d,%d)\n",parser.definition_cache[i].name,
               parser.definition_cache[i].tile.tile, parser.definition_cache[i].tile.color,
               parser.definition_cache[i].tile.solid, parser.definition_cache[i].tile.visible);
    };
    printf("-------\n");
#endif
    free( parser.str_cache );
    free( parser.rule_cache );
    free( parser.definition_cache );
}

void *check_table_size( void *cache, int cache_lines,
                        int *cache_size, int element_size )
{
    if (cache_lines > (*cache_size)-1)
    {
        (*cache_size) *= 2;
        cache = realloc(cache, (*cache_size) * element_size);
    }
    return cache;
}
