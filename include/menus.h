#ifndef MENUS_H
#define MENUS_H

#include "grid.h"

void move_selection(WINDOW *window, int *option, int direction, int left, int right, int pair);
int menu(WINDOW *menu_win, int menu_y, char **options, int color_pair);
void move_page(WINDOW *window, int *option, int direction, int *temp_len, char *pages[], int pages_init);
void inventory();

#endif /* MENUS_H */
