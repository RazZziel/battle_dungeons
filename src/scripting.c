#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "scripting.h"
#include "lex.yy.h"

#define INITIAL_CACHE_SIZE 2

extern int yyparse();
extern char *yyfilename;

game_parser_t parser;

void init_parser()
{
    /* Initialization of caches */
    parser.map_number = 0;
    parser.map_level = 0;

    parser.str_cache_size = INITIAL_CACHE_SIZE;
    parser.str_cache = malloc(parser.str_cache_size * sizeof(*parser.str_cache));

    parser.rule_cache_global_lines = 0;
    parser.rule_cache_size = INITIAL_CACHE_SIZE;
    parser.rule_cache = malloc(parser.rule_cache_size * sizeof(*parser.rule_cache));

    parser.material_cache_lines = 0;
    parser.material_cache_size = INITIAL_CACHE_SIZE;
    parser.material_cache = malloc(parser.material_cache_size * sizeof(*parser.material_cache));
}

void free_parser()
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
    for(int i=0;i<parser.material_cache_lines;i++)
    {
        printf("%s=(%c,%d,%d,%d)\n",parser.material_cache[i].name,
               parser.material_cache[i].tile.tile, parser.material_cache[i].tile.color,
               parser.material_cache[i].tile.solid, parser.material_cache[i].tile.visible);
    };
    printf("-------\n");
#endif
    free( parser.str_cache );
    free( parser.rule_cache );
    free( parser.material_cache );
}

int parse_script(char *filename)
{
    /* Calls yyparse on 'filename'.
     * Returns:
     *     0 on success
     *     1 on failure */

    int res;
    FILE *file = fopen(filename, "r");

    if ( file < 0 )
    {
        perror("Error");
        return 1;
    }
    yyrestart(file);
    yyset_lineno(1);
    yyfilename = filename;

    printf("Parsing %s\n", filename);

    res = yyparse();

    fclose(file);
    if (res != 0)
    {
        fprintf( stderr, "Could not load %s\n", filename );
        return 1;
    }

    return 0;
}
