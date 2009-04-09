/*
 * Control de mensajes de error
 */

#include <stdlib.h>
#include <ncurses.h>

#include "errors.h"

void die(int code, char *item, char *filename, int line)
{
    endwin();
    switch (code)
    {
    case BD_LOAD_ERROR:
        printf( "*** No se ha podido cargar la base de datos de %s.\n", item );
        printf( "*** Revisa los permisos de %s.\n", filename );
        break;
    case COLORS_ERROR:
        printf( "*** No se han podido inicializar los colores.\n" );
        printf( "*** Revisa la configuración de tu terminal.\n" );
        break;
    case BD_PARSE_ERROR:
        printf( "*** Error en la base de datos.\n" );
        printf( "*** %s: %i\n", filename, line );
        break;
    default:
        break;
    }
    printf( "\n*** Que tenga un buen día...\n\n" );
    exit(1);
}
