#include "global.h"
#include "parser.h"
#include "symtable.h"
#include "lex.yy.h"

extern int yyparse();
extern char *yyfilename;

game_parser_t parser;

void init_parser()
{
    /* Initialization of caches */
    memset( &parser, 0, sizeof(parser) );

    init_symbol_tables();
}

void free_parser()
{
    free_symbol_tables();
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
