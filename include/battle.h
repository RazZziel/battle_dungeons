#ifndef BATTLE_H
#define BATTLE_H

#include <ncurses.h>
#include "grid.h"

#define AUTHOR "RazZziel"
#define VERSION "0.1.0"


typedef struct {
    grid_t *loaded_grids[8],
        *current_grid;
    WINDOW *message_win,
        *game_win,
        *stats_win;
    pc_t pc,
        *npcs;
    int n_npcs;
} game_engine_t;


#endif /* BATTLE_H */
