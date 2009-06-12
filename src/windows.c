#include <ncurses.h>
#include <stdarg.h>
#include "battle.h"
#include "windows.h"

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

void prompt(char *title, char **text)
{
    WINDOW * win;
    char **line;
    int width, height;

    width = height = 0;
    line = (char**)text;
    while (*line != NULL)
    {
        int line_length = strlen( *(line++) );
        if (width < line_length)
            width = line_length;
        ++height;
    }
    width += 8;
    height += 5;

    win = newwin( height, width, (LINES-height)/2, (COLS-width)/2 );
    get_focus( win );

    wattron( win, A_BOLD );
    mvwprintw( win, 1, (width-strlen(title))/2, title );
    wattroff( win, A_BOLD );

    line = (char**)text;
    for (int i=3; *line != NULL; i++)
    {
        mvwprintw(win, i, 4, *(line++));
    }

    wattron( win, A_BOLD );
    mvwprintw( win, height-2, width-20, "Press any key..." );
    wattroff( win, A_BOLD );

    wgetch( win );
    destroy_win( win );
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
