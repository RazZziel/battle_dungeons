#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <math.h>
#include <popt.h>
#include <assert.h>
#include <time.h>

/*
 * Data files
 */

#define DATA_DIR          "../data/"
#define SCRIPT_ENTITIES   DATA_DIR"entities.rpg"
#define SCRIPT_MATERIALS  DATA_DIR"materials.rpg"
#define SCRIPT_MAPS       DATA_DIR"maps.rpg"

#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(x,y) ((x) < (y) ? (x) : (y))
#define plur(x) (((x) == 1) ? "" : "s")

#define nh 25
#define nv 20

#endif /* GLOBAL_H */
