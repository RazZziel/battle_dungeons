#include <ncurses.h>
#include <math.h>
#include <stdlib.h>

#include "grid.h"
#include "windows.h"
#include "colors.h"
#include "global.h"
#include "battle.h"

extern game_engine_t game;/* (Y) */

static inline int center_x(grid_t *grid, int x) { return x + ((game.game_win->_maxx - grid->width) / 2); }
static inline int center_y(grid_t *grid, int y) { return y + ((game.game_win->_maxy - grid->height) / 2); }

void visibility_area()
{
    for (int j=max(0, (game.pc.y)-game.pc.range_sight);
         j<min(game.current_grid->height, (game.pc.y)+game.pc.range_sight);
         j++)
    {
        for (int i=max(0, (game.pc.x)-game.pc.range_sight);
             i<min(game.current_grid->width, (game.pc.x)+game.pc.range_sight);
             i++)
        {
            if ( ((pow(i-(game.pc.x), 2) + pow(j-(game.pc.y), 2)) <= pow(game.pc.range_sight, 2)) &&
                 (!grid_node(game.current_grid, j, i)->visible) )
            {
                grid_node(game.current_grid, j, i)->visible = TRUE;
            }
        }
    }
}

grid_node_t *grid_node(grid_t *grid, int y, int x)
{
    return grid->grid + (y * grid->width) + (x);
}

void new_grid(grid_t **grid, int height, int width)
{
    (*grid) = malloc( sizeof(grid_t) );
    (*grid)->height = height;
    (*grid)->width = width;
    (*grid)->grid = malloc( height * width * sizeof(grid_node_t) );
}

inline void draw_node(int y, int x)
{
    /* Traverse list of superimposed tiles, as in ncurses
       we don't need to render them in order.
       TODO: Probably should reverse list order. */
    grid_node_t *node = NULL;
    grid_node_t *iter;
    for ( iter = grid_node(game.current_grid, y, x);
          iter != NULL;
          iter = iter->above )
    {
        if ( iter->visible )
        {
            node = iter;
        }
    }

    if (node != NULL)
    {
        wattron( game.game_win, COLOR_PAIR(node->color) );
        mvwprintw( game.game_win, center_y(game.current_grid,y), center_x(game.current_grid,x), "%c", node->tile );
    }
    else
    {
        mvwprintw( game.game_win, center_y(game.current_grid,y), center_x(game.current_grid,x), " " );
    }
}

inline void draw_pc(pc_t *pc)
{
    if (grid_node(game.current_grid, pc->y, pc->x)->visible)
    {
        wattron( game.game_win, COLOR_PAIR(pc->color) );
        mvwprintw( game.game_win, center_y(game.current_grid, pc->y),
                   center_x(game.current_grid,pc->x), "%c", pc->avatar );
        wrefresh( game.game_win );
    }
}

void draw_grid()
{
    for (int j=0; j<game.current_grid->height; j++)       /* Y */
    {
        for (int i=0; i<game.current_grid->width; i++)    /* X */
        {
            draw_node(j, i);
        }
    }

    wrefresh( game.game_win );
}
