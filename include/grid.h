#ifndef GRID_H
#define GRID_H

#include "character.h"

typedef struct {
  char type;
  int color;
  int solid; /* boolean */
  int visible;
  int special;
} grid_node_t;

typedef struct {
  grid_node_t *grid;
  int height;
  int width;
} grid_t;

#define PJ           '@'
#define ENE_SKL      'z' //gray
#define ENE_KOBOLD   'k' //green
#define ENE_ORC      'o' //green
#define ENE_GLOBIN   'g' //blue


#define TER_WATHER   '=' //blue
#define TER_ROAD     '.' //brown
#define TER_GRASS    '.' //green
#define OBS_ROCK     'M'
#define OBS_WALL     '#' //gray
#define OBS_TREE     'T' //green
#define OBS_O_DOOR   '/' // brown
#define OBS_C_DOOR   '+' // brown

#define OBSTACLE
#define MAP_FOREST   '&' //green
#define MAP_PLAINS   '"' //green
#define MAP_HILS     '~' //brown
#define MAP_ROAD     '.' //brown
#define MAP_PEAK     '^' //brown/white
#define OBJ_WOOD_WPN '(' //brown
#define OBJ_METL_WPN '(' //grey

#define TER_GRASS_COLOR 20
#define OBS_WALL_COLOR 30

void visibility_area(grid_t *grid, pj_t *pj);
void create_first_combat_grid(grid_t *grid, pj_t *pj, pj_t *enemy);
grid_node_t * grid_node(grid_t *grid, int y, int x);
void new_grid(grid_t **grid, int height, int width);
void draw_node (grid_t *grid, int y, int x);
void draw_pj (grid_t *grid, pj_t *pj);
void draw_grid (grid_t *grid, pj_t *pj, pj_t *enemy);


#endif /* GRID_H */
