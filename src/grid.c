#include <ncurses.h>
#include <math.h>
#include <stdlib.h>

#include "grid.h"
#include "windows.h"
#include "colors.h"
#include "global.h"

void
visibility_area(grid_t *grid, pj_t *pj) {
  int j, i;
  /*  for (j=max(0, (pj->y)-10); j<min(LINES-7, (pj->y)+11); j++)
      for (i=max(0, (pj->x)-10); i<min(COLS, (pj->x)+11); i++) {
      grid_node(grid, j, i)->visible = TRUE;
      draw_node(grid, j, i);
      }*/

for (j=max(0, (pj->y)-10); j<min(LINES-7, (pj->y)+11); j++)
  for (i=max(0, (pj->x)-10); i<min(COLS, (pj->x)+11); i++) {
    if ( pow(i-(pj->x), 2) + pow(j-(pj->y), 2) <= pow(5, 2) && !grid_node(grid, j, i)->visible) {
      grid_node(grid, j, i)->visible = TRUE;
      draw_node(grid, j, i);
    }
  }

}

void
create_first_combat_grid(grid_t *grid, pj_t *pj, pj_t *enemy) {
  int i, j;
  
  for (j=0; j<grid->height; j++)
    for (i=2; i<grid->width-2; i++) {
      grid_node(grid, j, i)->type = TER_GRASS;
      grid_node(grid, j, i)->color = TER_GRASS_COLOR;
      grid_node(grid, j, i)->solid = FALSE;
      grid_node(grid, j, i)->visible = FALSE;
    }
  
  for (j=0; j<grid->height; j++) {
    grid_node(grid, j, 1)->type = OBS_WALL;
    grid_node(grid, j, 1)->color = OBS_WALL_COLOR;
    grid_node(grid, j, 1)->solid = TRUE;
    grid_node(grid, j, 1)->visible = FALSE;
    
    grid_node(grid, j, grid->width-2)->type = OBS_WALL;
    grid_node(grid, j, grid->width-2)->color = OBS_WALL_COLOR;
    grid_node(grid, j, grid->width-2)->solid = TRUE;
    grid_node(grid, j, grid->width-2)->visible = FALSE;
  }
  
  for (j=1; j<=grid->height*3/4; j++) {
    grid_node(grid, j, grid->width/3)->type = OBS_WALL;
    grid_node(grid, j, grid->width/3)->color = OBS_WALL_COLOR;
    grid_node(grid, j, grid->width/3)->solid = TRUE;
    grid_node(grid, j, grid->width/3)->visible = FALSE;
  }
  for (j=grid->height/4; j<grid->height; j++) {
    grid_node(grid, j, grid->width*2/3)->type = OBS_WALL;
    grid_node(grid, j, grid->width*2/3)->color = OBS_WALL_COLOR;
    grid_node(grid, j, grid->width*2/3)->solid = TRUE;
    grid_node(grid, j, grid->width*2/3)->visible = FALSE;
  }

  for (i=1; i<grid->width-2; i++) {
    grid_node(grid, 0, i)->type = OBS_WALL;
    grid_node(grid, 0, i)->color = OBS_WALL_COLOR;
    grid_node(grid, 0, i)->solid = TRUE;
    grid_node(grid, 0, i)->visible = FALSE;

    grid_node(grid, grid->height-1, i)->type = OBS_WALL;
    grid_node(grid, grid->height-1, i)->color = OBS_WALL_COLOR;
    grid_node(grid, grid->height-1, i)->solid = TRUE;
    grid_node(grid, grid->height-1, i)->visible = FALSE;
  }

  do {
  enemy->x = rand()%(grid->width-1)+1;
  enemy->y = rand()%(grid->height/2-1)+1;
  } while (grid_node(grid, enemy->y, enemy->x)->solid);
  do {
  pj->x = rand()%(grid->width-1)+1;
  pj->y = rand()%(grid->height/2-1) + 1 + (grid->height/2);
  } while (grid_node(grid, enemy->y, enemy->x)->solid);
  pj->status.blind=0;
  pj->playable=1;
  enemy->playable=0;

  visibility_area(grid, pj);

}

grid_node_t
*grid_node(grid_t *grid, int y, int x) {
  return grid->grid+y*grid->width+x;
}

void
new_grid(grid_t **grid, int height, int width) {
  *grid = (grid_t*)malloc(sizeof(grid_t));
  (*grid)->height = height;
  (*grid)->width = width;
  (*grid)->grid = (grid_node_t*)malloc(height*width*sizeof(grid_node_t));
}

void
draw_node (grid_t *grid, int y, int x) {
  if (grid_node(grid, y, x)->visible) {
    wattron(game_win, COLOR_PAIR(grid_node(grid, y, x)->color));
    mvwprintw(game_win, y, x, "%c", grid_node(grid, y, x)->type);
  } else {
    mvwprintw(game_win, y, x, " ");
  }
  wrefresh(game_win);
}

void
draw_pj (grid_t *grid, pj_t *pj) {
  if (grid_node(grid, pj->y, pj->x)->visible) {
    wattron(game_win, COLOR_PAIR(pj->color));
    mvwprintw( game_win, pj->y, pj->x, "%c", pj->avatar );
    wrefresh(game_win);
  }
}

void
draw_grid (grid_t *grid, pj_t *pj, pj_t *enemy) {
  int j, i;
  
  for ( j=0; j<grid->height; j++)     /* Y */
    for ( i=0; i<grid->width; i++) {  /* X */
      if (grid_node(grid, j, i)->visible) {
	wattron(game_win, COLOR_PAIR(grid_node(grid, j, i)->color));
	mvwprintw( game_win, j, i, "%c", grid_node(grid, j, i)->type );
      }
    }

  wattron(game_win, COLOR_PAIR(10));
  mvwprintw( game_win, pj->y, pj->x, "%c", pj->avatar );
  if (grid_node(grid, enemy->y, enemy->x)->visible) {
    wattron(game_win, COLOR_PAIR(40));
    mvwprintw( game_win, enemy->y, enemy->x, "%c", enemy->avatar );
  }

  wrefresh(game_win);
}
