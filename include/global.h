#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>  // usleep
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <ncurses.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>

#ifdef HAVE_POPT /*TODO*/
# include <popt.h>
#endif


/* Data files */

#define DATA_DIR         "../data/"
#define SCRIPT_ENTITIES  DATA_DIR"entities.rpg"
#define SCRIPT_MATERIALS DATA_DIR"materials.rpg"
#define SCRIPT_MAPS      DATA_DIR"maps.rpg"


#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(x,y) ((x) < (y) ? (x) : (y))
#define plur(x)  (((x) == 1) ? "" : "s")

#define nh 25
#define nv 20

#define AUTHOR "RazZziel"
#define VERSION "0.1.0"


#endif /* GLOBAL_H */
