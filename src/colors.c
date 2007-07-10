/* Inicializacion de los colores usados en el juego. */

#include <ncurses.h>
#include "colors.h"

int
init_colors()
{
  if (has_colors() /*&& can_change_color()*/) {
    start_color();    

    /* Inicializacion de colores */
    init_color(COLOR_GREY, 700, 0, 0);

    /* Inicializacion de pares */
      // Colores basicos
    init_pair(10, COLOR_RED,     COLOR_BLACK);
    init_pair(20, COLOR_GREEN,   COLOR_BLACK);
    init_pair(30, COLOR_YELLOW,  COLOR_BLACK);
    init_pair(40, COLOR_BLUE,    COLOR_BLACK);
    init_pair(50, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(60, COLOR_CYAN,    COLOR_BLACK);
    init_pair(70, COLOR_WHITE,   COLOR_BLACK);
      // Colores personalizados
    init_pair(80, COLOR_GREY,    COLOR_BLACK);

    return 1;
  } else return 0;
}
