/*
 * Control de mensajes de error
 */

#include <stdlib.h>
#include <stdarg.h>
#include <ncurses.h>

#include "errors.h"

void die(const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
  
    vdie( fmt, argp );

    va_end(argp);
}

void vdie(const char *fmt, va_list argp)
{
    endwin();

    vfprintf( stderr, fmt, argp );

    exit(1);
}
