#ifndef WINDOWS_H
#define WINDOWS_H

#include <ncurses.h>

extern WINDOW *game_win;

void generate_game_screen();
void message(const char *fmt, ...);
void destroy_win(WINDOW *window);
void get_focus(WINDOW *window);
void loose_focus(WINDOW  *window);

#endif /* WINDOWS_H */
