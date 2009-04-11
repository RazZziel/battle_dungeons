#ifndef BATTLE_H
#define BATTLE_H

#include <ncurses.h>
#include "grid.h"

#define AUTHOR "RazZziel"
#define VERSION "0.1.0"


typedef struct {
    grid_t *grid[8];
    WINDOW *message_win, *game_win, *stats_win;
} game_engine_t;


#endif /* BATTLE_H */
