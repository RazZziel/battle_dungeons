#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "lex.yy.h"

extern char *yyfilename;
extern int yyparse();

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

    res = yyparse();
    fclose(file);
    if (res != 0)
    {
        fprintf( stderr, "Could not load %s\n", filename );
        return 1;
    }

    return 0;
}
