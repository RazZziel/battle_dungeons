#include "global.h"
#include "errors.h"

#include <execinfo.h>

void print_trace()
{
    void *array[10];
    size_t size;
    char **strings;
    size_t i;
     
    size = backtrace( array, 10 );
    strings = backtrace_symbols( array, size );
     
    printf( "  Obtained %zd stack frames.\n", size );
     
    for (i = 0; i < size; i++)
        printf( "   %s\n", strings[i] );
     
    free( strings );
}


void __die__(const char *file, const int line, const char *function, const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
  
    __vdie__( file, line, function, fmt, argp );

    va_end(argp);
}

void __vdie__(const char *file, const int line, const char *function, const char *fmt, va_list argp)
{
    endwin();

    fprintf( stderr, "%s:%d[%s()]: error: ", file, line, function );

    vfprintf( stderr, fmt, argp );

    print_trace();

    fflush( stderr );
    exit( 1 );
}
