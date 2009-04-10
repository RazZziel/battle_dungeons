#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern char *yyfilename;

int main(int argc, char *argv[])
{
    int i;
    for (i=1; i<argc; i++)
    {
        FILE *file = fopen(argv[i], "r");
        if ( file < 0 )
        {
            perror("Error");
        }
        else
        {
            int res;

            yyrestart(file);
            yyfilename = argv[i];

            printf("Parsing %s\n", yyfilename);

            res = yyparse();
            fclose(file);
            if (res != 0)
            {
                fprintf( stderr, "Could not load %s\n", yyfilename );
            }
        }
    }
}
