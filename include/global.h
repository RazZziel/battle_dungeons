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

#define DATA      "data"           /* directory where data files lay */
#define EXT       "db"             /* file extension for data files */
#define ENEMIGOS  "enemigos."+EXT  /* enemy database */
#define ARMAS     "armas."+EXT     /* weapon database */
#define HECHIZOS  "hechizos."+EXT  /* spell database */
#define CLASES    "clases."+EXT    /* class database */
#define RAZAS     "razas."+EXT     /* race database */

#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(x,y) ((x) < (y) ? (x) : (y))
#define plur(x) (((x) == 1) ? "" : "s")

#define nh 25
#define nv 20

#endif /* GLOBAL_H */
