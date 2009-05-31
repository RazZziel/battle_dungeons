#include "character.h"


void inventory_add(character_t *character, object_t *object)
{
    character->objects[character->n_objects++] = object;
}

void inventory_del(character_t *character, object_t *object)
{
    ;
}

void inventory_equip(character_t *character, object_t *object)
{
    ;
}

void inventory_unequip(character_t *character, object_t *object)
{
    ;
}
