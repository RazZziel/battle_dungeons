#ifndef COMBAT_H
#define COMBAT_H

#include "character.h"

#define modifier(score) (score/2)-5

void combate (pj_t *pj, pj_t *enemy);
void new_combat();

#endif /* COMBAT_H */
