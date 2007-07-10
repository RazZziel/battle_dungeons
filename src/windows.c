#include <ncurses.h>
#include <stdarg.h>
//#include "battle.h"

WINDOW *message_win, *game_win, *stats_win;

void
generate_game_screen() {
  message_win = newwin(3      , COLS, 0      , 0);
  game_win    = newwin(LINES-7, COLS, 3      , 0);
  stats_win   = newwin(4      , COLS, LINES-4, 0);
}

void
message(const char *fmt, ...) {
  va_list argp;
  va_start(argp, fmt);
  
  wmove(message_win, 1, 1);
  vwprintw(message_win, fmt, argp);
  
  va_end(argp);
  wrefresh(message_win);
}

void
destroy_win (WINDOW *window) {	
  wborder(window,' ',' ',' ',' ',' ',' ',' ',' ');
  werase(window);
  wrefresh(window);
  delwin(window);
}

void
get_focus (WINDOW *window) {
  wattron(window, COLOR_PAIR(10));
  wborder(window, 0, 0, ' ', ' ', '/', '\\', '\\', '/');
  wattroff(window, COLOR_PAIR(10));
  wrefresh(window);
}

void
loose_focus (WINDOW  *window) {
  wattron(window, COLOR_PAIR(40));
  wborder(window, ' ', ' ', ' ', ' ', '/', '\\', '\\', '/');
  wattroff(window, COLOR_PAIR(40));
  wrefresh(window);
}
