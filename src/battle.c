/*  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *
 *                 *  *       BATTLE DUNGEONS       *  *                 *
 *                    *             ~               *                    *
 *                                                                       *
 * Versión         : 0.1.0                                               *
 * Autor           : Raziel ex-Seraphan                                  *
 * Fecha de inicio : 03.02.2005 (víspera de primera convocatoria de FMC) *
 * Portado a FP    : 08.03.2005                                          *
 * Portado a C     : 15.06.2005 (sí, exámenes...)                        *
 * Portado a C++   : 27.06.2006 (adivinas? xDDDD)                        *
 * Portado a C     : 07.07.2007 (seh, era una gilipollez...)             *
 *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  */

#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <popt.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>  // usleep

#include "menus.h"
#include "windows.h"
#include "colors.h"
#include "battle.h"
#include "errors.h"
#include "combat.h"
#include "story.h"

void
intro() {
/* ph34r my l33t sk1llz */
  char *main_title[] = {
                        " /\\",
                        "|| |",
                        "|| |                 ____",
                        "|| |                / ,\\''\\",
                        "|| |_ ------------------|''|",
                        "|| |_|  BATTLE DUNGEONS |''|",
                        "|| |  ~~~~~~~~~~~~~~~~~/''/",
                        "|| |                 /''/",
                        "|| |                (''/",
                        "|| |                 \\/",
                        "'' '",
                       };
  int i, y=3, x=(COLS/2-13);
  int n_lineas = sizeof(main_title) / sizeof(char *);
  
  erase();
  mvprintw( 3, 5, "Raziel Productions Presents" );
  refresh();
  for( i=0; i<20; i++ ) {
      usleep( 100000 );
      printw( "." );
      refresh();
    }
  erase();

  usleep(150000);
  for (i=0; i<n_lineas; i++) {
    mvprintw( y++, x, "%s", main_title[i] );
    refresh();
    usleep(100000);
  }
  usleep(200000);
}

void
poweroff() {
  endwin();
  printf( "\nBattle Dungeons v.%s\n", VERSION );
  printf( "Raziel Productions 2005\n" );
  printf( "\n                -Yo soy goma. Tú eres cola.\n\n" );
  endwin();             // Desgarga de ncurses
}

void
main_menu() {
  
#define MENU_WIDTH  48
#define MENU_HEIGHT 20

  WINDOW *main_menu_win;
  char *menu_options[] = {
                          "Modo Arcade",
                          "Modo Historia",
                          "Salir",
			  "",
                         };
  int option, y, x;
  assert(strlen(menu_options[(sizeof(menu_options) / sizeof(char *))-1]) == 0);
  
  do {
    main_menu_win = newwin( MENU_HEIGHT, MENU_WIDTH, (LINES-MENU_HEIGHT)*3/4, (COLS-MENU_WIDTH)/2 );
    get_focus( main_menu_win );
    x = (MENU_WIDTH-30)/2;
    y = 5;
    wattron( main_menu_win, COLOR_PAIR(30) );
    mvwprintw( main_menu_win,   y, x, " /*)                      (*\\" );
    mvwprintw( main_menu_win, ++y, x, "|*(     Menú Principal     )*|" );
    mvwprintw( main_menu_win, ++y, x, " \\*________________________*/" );
    
    option = menu( main_menu_win, y+4, menu_options, MAIN_MENU_PAIR );
    loose_focus( main_menu_win );
    switch( option )
      {
      case 1:
	new_combat();
	break;
      case 2:
	story_mode();
	break;
      default:
	break;
      }
    get_focus( main_menu_win );
  } while( option != 3 );
  delwin(main_menu_win);
}

int
main(int argc, char *argv[]) {
  /* command line options parsing */

  bool * argLol = FALSE;
  int rc;

  struct poptOption optionsTable[] = {
    {"lol", 'l', POPT_ARG_NONE, &argLol, TRUE, "Display this text lol PONIES", NULL},
    POPT_AUTOHELP
    POPT_TABLEEND
  };
  poptContext context = poptGetContext( NULL, argc, (const char **) argv, optionsTable, 0 );
  while ((rc = poptGetNextOpt(context)) > 0);

  /* random libraries initialization */

  srandom((unsigned int) time(NULL));

  /* data libraries loading */

  //init_enemies();

  /* graphic library */

  initscr();            // Inicializacion de ncurses
  curs_set(0);          // Cursor invisible
  if (init_colors()==0) // Inicializacion de colores
    do_error(COLORS_ERROR, "", "", 0);
    
  /* game */

  intro();
  main_menu();
  poweroff();
  return 0;
}
