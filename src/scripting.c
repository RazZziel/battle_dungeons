#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "scripting.h"
#include "lex.yy.h"

extern int yyparse();
extern char *yyfilename;

game_parser_t parser;

void init_cache()
{
    /* Initialization of caches */
    parser.map_number = 0;
    parser.map_level = 0;

    parser.str_cache_size = 2;
    parser.str_cache = malloc(parser.str_cache_size * sizeof(*parser.str_cache));

    parser.rule_cache_global_lines = 0;
    parser.rule_cache_size = 2;
    parser.rule_cache = malloc(parser.rule_cache_size * sizeof(*parser.rule_cache));
}

void free_cache()
{
    free( parser.str_cache );
    free( parser.rule_cache );
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
    yyfilename = filename;

    printf("Parsing %s\n", filename);

    init_cache();
    res = yyparse();
    free_cache();

    fclose(file);
    if (res != 0)
    {
        fprintf( stderr, "Could not load %s\n", filename );
        return 1;
    }

    return 0;
}
