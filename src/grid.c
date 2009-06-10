#include <ncurses.h>
#include <math.h>
#include <stdlib.h>

#include "grid.h"
#include "windows.h"
#include "color.h"
#include "global.h"
#include "battle.h"

extern game_engine_t game;

static inline int center_x(grid_t *grid, int x)
{
    return x + ( (game.game_win->_maxx > grid->width)
                 ? ((game.game_win->_maxx - grid->width) / 2)
                 : (game.grid_x_offset));
}
static inline int center_y(grid_t *grid, int y)
{
    return y + ( (game.game_win->_maxy >= grid->height)
                 ? ((game.game_win->_maxy - grid->height) / 2)
                 : (game.grid_y_offset));
}

static inline bool in_sight_range(entity_t *entity, int y, int x)
{
    return ( ( pow(x-entity->x, 2) + pow(y-entity->y, 2) )
             <= pow(entity->data.character->range_sight, 2) );
}

void visibility_area()
{
    for (int j=max(0, (game.pc->y)-game.pc->data.character->range_sight);
         j<min(game.current_grid->height, (game.pc->y)+game.pc->data.character->range_sight);
         j++)
    {
        for (int i=max(0, (game.pc->x)-game.pc->data.character->range_sight);
             i<min(game.current_grid->width, (game.pc->x)+game.pc->data.character->range_sight);
             i++)
        {
            if ( in_sight_range(game.pc, j, i) &&
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

    if (node)
    {
        wattron( game.game_win,
                 ( in_sight_range(game.pc, y, x)
                   ? COLOR_PAIR(node->color) | A_BOLD
                   : COLOR_PAIR(8) | A_DIM ) );

        mvwprintw( game.game_win,
                   center_y(game.current_grid,y),
                   center_x(game.current_grid,x),
                   "%c", node->tile );
    }
    else
    {
        mvwprintw( game.game_win,
                   center_y(game.current_grid,y),
                   center_x(game.current_grid,x),
                   " " );
    }
}

inline void draw_entity(entity_t *pc)
{
    int y_offset = max(game.game_win->_maxy-game.current_grid->height+1,
                       min(0, game.game_win->_maxy/2 - game.pc->y));
    int x_offset = max(game.game_win->_maxx-game.current_grid->width+1,
                       min(0, game.game_win->_maxx/2 - game.pc->x));
    if ((y_offset != game.grid_y_offset) ||
        (x_offset != game.grid_x_offset))
    {
        game.grid_y_offset = y_offset;
        game.grid_x_offset = x_offset;
        draw_grid();
    }

    if (!in_sight_range(game.pc, pc->y, pc->x))
    {
        draw_node(pc->y, pc->x);
    }
    else if (grid_node(game.current_grid, pc->y, pc->x)->visible)
    {
        wattron( game.game_win, COLOR_PAIR(pc->color) );
        mvwprintw( game.game_win,
                   center_y(game.current_grid, pc->y),
                   center_x(game.current_grid, pc->x),
                   "%c", pc->tile );
        wrefresh( game.game_win );
    }
}

void draw_grid()
{
    int ji = max(0,-game.grid_y_offset);
    int ii = max(0,-game.grid_x_offset);

    for (int j=ji;
         j<ji+min(game.current_grid->height,
                  game.game_win->_maxy+1);
         j++)
    {
        for (int i=ii;
             i<ii+min(game.current_grid->width,
                      game.game_win->_maxx+1);
             i++)
        {
            draw_node(j, i);
        }
    }

    wrefresh( game.game_win );
}
