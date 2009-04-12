#ifndef GRID_H
#define GRID_H

#include "global.h"
#include "character.h"

typedef struct grid_node_s {
    char tile;
    int color;
    bool solid;
    bool visible;
    struct grid_node_s *above;
} grid_node_t;

typedef struct {
    grid_node_t *grid;
    int height;
    int width;
} grid_t;

#define PJ              '@'
#define TER_GRASS       '.'
#define OBS_WALL        '#'
#define TER_GRASS_COLOR 20
#define OBS_WALL_COLOR  30

void visibility_area(grid_t *grid, pj_t *pj);
void create_first_combat_grid(grid_t *grid, pj_t *pj, pj_t *enemy);
void new_grid(grid_t **grid, int height, int width);
void draw_pj(grid_t *grid, pj_t *pj);
void draw_grid (grid_t *grid, pj_t *pj, pj_t *enemy);
grid_node_t * grid_node(grid_t *grid, int y, int x);
void draw_node(grid_t *grid, int y, int x);

#endif /* GRID_H */
