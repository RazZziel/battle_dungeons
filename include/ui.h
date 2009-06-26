#ifndef WINDOWS_H
#define WINDOWS_H

#include "global.h"

extern WINDOW *game_win;

void generate_game_screen();
void message(const char *fmt, ...);
void prompt(char *title, char **text);
void destroy_win(WINDOW *window);
void get_focus(WINDOW *window);
void drop_focus(WINDOW *window);

#endif /* WINDOWS_H */
