#include "global.h"
#include "parser.h"
#include "lex.yy.h"

#define INITIAL_CACHE_SIZE 2

extern int yyparse();
extern char *yyfilename;

game_parser_t parser;

void init_parser()
{
    /* Initialization of caches */
    memset( &parser, 0, sizeof(parser) );

    parser.str_cache_size = INITIAL_CACHE_SIZE;
    parser.str_cache = malloc(parser.str_cache_size * sizeof(*parser.str_cache));

    parser.rule_cache_size = INITIAL_CACHE_SIZE;
    parser.rule_cache = malloc(parser.rule_cache_size * sizeof(*parser.rule_cache));

    parser.definition_cache_size = INITIAL_CACHE_SIZE;
    parser.definition_cache = malloc(parser.definition_cache_size * sizeof(*parser.definition_cache));
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

    fprintf( stderr, "Parsing %s\n", filename );

    res = yyparse();

    fclose(file);
    if (res != 0)
    {
        fprintf( stderr, "Could not load %s\n", filename );
        return 1;
    }

    return 0;
}
