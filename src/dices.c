#include <stdlib.h>

int dices (int quantity, int faces, int modifiers)
{
    int i, temp = 0;

    for( i=0; i<quantity; i++)
    {
        temp += ( (rand()%faces) + 1 );
    }
    return temp + modifiers;
}
