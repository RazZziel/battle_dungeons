#include <ncurses.h>
#include <stdarg.h>
#include "battle.h"

extern game_engine_t game;

void generate_game_screen()
{
  game.message_win = newwin(3      , COLS, 0      , 0);
  game.game_win    = newwin(LINES-7, COLS, 3      , 0);
  game.stats_win   = newwin(4      , COLS, LINES-4, 0);
}

void message(const char *fmt, ...)
{
  va_list argp;
  va_start(argp, fmt);
  
  wmove(game.message_win, 1, 1);
  vwprintw(game.message_win, fmt, argp);
  
  va_end(argp);
  wrefresh(game.message_win);
}

void destroy_win (WINDOW *window)
{	
  wborder(window,' ',' ',' ',' ',' ',' ',' ',' ');
  werase(window);
  wrefresh(window);
  delwin(window);
}

void get_focus (WINDOW *window)
{
  wattron(window, COLOR_PAIR(1));
  wborder(window, 0, 0, ' ', ' ', '/', '\\', '\\', '/');
  wattroff(window, COLOR_PAIR(1));
  wrefresh(window);
}

void drop_focus (WINDOW  *window)
{
  wattron(window, COLOR_PAIR(4));
  wborder(window, ' ', ' ', ' ', ' ', '/', '\\', '\\', '/');
  wattroff(window, COLOR_PAIR(4));
  wrefresh(window);
}
