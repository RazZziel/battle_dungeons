#include "global.h"
#include "errors.h"

#include <execinfo.h>

void print_trace()
{
    void *array[30];
    size_t size;
    char **strings;
    size_t i;

    size = backtrace( array, 30 );
    strings = backtrace_symbols( array, size );

    printf( " |Backtrace:\n" );

    for (i = 3; i < size; i++)
    {
        printf( " |  %s\n", strings[i] );
    }

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

    fprintf( stderr, "\n%s:%d [%s()]: error: ", file, line, function );

    vfprintf( stderr, fmt, argp );

#if 1
    print_trace();
#endif

    fprintf( stderr, "\n" );
    fflush( stderr );

    exit( 1 );
}

void __debug__(const char *file, const int line, const char *function, const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);

    fprintf( stderr, "%s:%d [%s()]: debug: ", file, line, function );

    vfprintf( stderr, fmt, argp );

    fprintf( stderr, "\n" );
    fflush( stderr );

    va_end(argp);
}

