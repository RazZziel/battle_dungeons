#include "global.h"

void
story_mode() {
  int i;
  erase();
  attron(COLOR_PAIR(4));
  mvprintw( 10, 10, "[LOADING]   " );
  refresh();
  usleep( 500000 );
  for (i=0; i<20; i++)
    {
      printw( "|" );
      refresh();
      usleep( 150000 );
    }
  mvprintw( 12, 20, "Venga ya..." );
  getch();
  erase();
}
