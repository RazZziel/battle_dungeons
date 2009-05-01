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

void visibility_area();
void create_first_combat_grid(grid_t *grid, entity_t *pc, entity_t *enemy);
void new_grid(grid_t **grid, int height, int width);
void draw_entity(entity_t *pc);
void draw_grid();
grid_node_t *grid_node(grid_t *grid, int y, int x);
void draw_node(int y, int x);

#endif /* GRID_H */
