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

#define PC              '@'
#define TER_GRASS       '.'
#define OBS_WALL        '#'
#define TER_GRASS_COLOR 20
#define OBS_WALL_COLOR  30

void visibility_area();
void create_first_combat_grid(grid_t *grid, pc_t *pc, pc_t *enemy);
void new_grid(grid_t **grid, int height, int width);
void draw_pc(pc_t *pc);
void draw_grid();
grid_node_t *grid_node(grid_t *grid, int y, int x);
void draw_node(int y, int x);

#endif /* GRID_H */
