#ifndef BATTLE_H
#define BATTLE_H

#define AUTHOR "Raziel ex-Seraphan"
#define VERSION "0.1.0"
#define LONGER_PARAM_LENGTH  6  /* Longitud del parámetro más largo */

#define HELP_PARAM "--help"

/*
 * Macros
 */

void intro();
void poweroff();
void main_menu();
int main(int argc, char *argv[]);

#endif /* BATTLE_H */
