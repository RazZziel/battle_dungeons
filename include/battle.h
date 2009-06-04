#ifndef BATTLE_H
#define BATTLE_H

#include <ncurses.h>
#include <stdbool.h>
#include "grid.h"

#define AUTHOR "RazZziel"
#define VERSION "0.1.0"


typedef struct {
    grid_t *loaded_grids[8],
        *current_grid;
    WINDOW *message_win,
        *game_win,
        *stats_win;
    entity_t *pc,
        **npcs,
        **entities;
    int n_npcs,
        n_entities;
    int grid_x_offset,
        grid_y_offset;
} game_engine_t;


#endif /* BATTLE_H */
