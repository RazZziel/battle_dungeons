/*
 * ROFLOLOL
 */

#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "battle.h"
#include "combat.h"
#include "windows.h"
#include "grid.h"
#include "menus.h"
#include "color.h"
#include "character.h"
#include "global.h"
#include "scripting.h"

extern game_engine_t game;

void attack(entity_t *attacker, entity_t *victim)
{
/* Ataca al enemigo.                                             *
 * Mediante una tirada de random sobre 20, determina si el golpe *
 * es normal, crítico o pifia.                                   *
 * La dureza del golpe es directamente proporcional a la fuerza  *
 * del pc y a la fuerza del arma, e inversamente proporcional    *
 * a la defensa del oponente.                                    */
//  bool critical = FALSE;
    int hit;
  
    //  text = "¡Atacas al " + enemy->name + " con tu " + "cepillodientes" + "!\n";
    //  message( "You attack %s", "random enemy" );
    if (attacker->type == ENTITY_PC)
        message("You attack the %s", victim->name);
    else
        message("The %s attacks you", attacker->name);
    // TODO ver lo de los ataques con rango
    //  if ( dices( 1,20,(base_attack_bonus + modifier( pc->str )+ size mod )
    //	      < ( 10 + armor bonus + shield bonus + modifier( enemy->dex ) + enemy->size )))
    //    message("Fallas el golpe*\n");
    //  else
    //    hit = dices( pc->arma->dado, 6, ( base atack bonus + modifier( enemy->str ) + enemy->tamanyo ));
  
    //  if (pifia)
    //    message( "%s%s se ríe de ti...\n", enemy->prefix, enemy->name );
    //  else if (critico)
    //    message( "¡¡Le asestas un golpe mortal a%s%s!!\n", enemy->prefix, enemy->name );
    hit = attacker->data.character->str;
    victim->data.character->hp -= hit;
}

void walk(int horizontal, int vertical, entity_t *pc)
{
    /* drunk characters detection */
    if (pc->data.character->status.drunk)
    {
        if (horizontal == 0)
        {
            horizontal = (rand()%3)-1;
        }
        else if (vertical == 0)
        {
            vertical = (rand()%3)-1;
        }
        else if ((horizontal != 0) && (vertical != 0))
        {
            horizontal = (rand()%2)*horizontal;
            vertical = (rand()%2)*vertical;
        }
    }

    int dest_x = pc->x + horizontal,
        dest_y = pc->y + vertical;

    bool collision =
        /* collision detection with grid */
        ( dest_x == game.current_grid->width )  ||
        ( dest_x == -1 )                        ||
        ( dest_y == game.current_grid->height ) ||
        ( dest_y == -1 )                        ||
    
        /* collision detection with obstacles */
        ( grid_node( game.current_grid, dest_y, dest_x )->solid == TRUE ) ||

        /* collision detection with player */

        ( (dest_x == game.pc->x) &&
          (dest_y == game.pc->y) );

    /* collision detection with enemy */
    for (int i=0; i<game.n_npcs; i++)
    {
        if ( (dest_x == game.npcs[i]->x) &&
             (dest_y == game.npcs[i]->y) &&
             (game.npcs[i]->data.character->status.alive) )
        {
            if (game.npcs[i]->data.character->aggressive)
            {
                attack( pc, game.npcs[i] );
            }
            collision = TRUE;
        }
    }

    if (!collision)
    {
        draw_node( pc->y, pc->x );
        pc->x += horizontal;
        pc->y += vertical;
    }
}

void combat_menu()
{
#define COMBAT_MENU_HEIGHT 10
#define COMBAT_MENU_WIDTH  20

    WINDOW *combat_menu_win;
    char *menu_options[] = {
        "(A)ttack",
        "(M)agic",
        "(I)nventory",
        "",
    };
  
    assert(strlen(menu_options[(sizeof(menu_options) / sizeof(char *))-1]) == 0);
    combat_menu_win = newwin(COMBAT_MENU_HEIGHT, COMBAT_MENU_WIDTH,
                             LINES-COMBAT_MENU_HEIGHT, COLS-COMBAT_MENU_WIDTH-3);
    get_focus(combat_menu_win);

    switch( menu(combat_menu_win, 1, menu_options, 6) )
    {
    case 1: break;
    case 2: break;
    case 3:
        touchwin( game.game_win );
        touchwin( game.stats_win );
        wrefresh( game.stats_win );
        inventory();
        break;
    default: break;
    }
    destroy_win( combat_menu_win );
}

void help_menu()
{
    char *help_text[] = {
        "  Movement",
        "",
        "    q   w   e      y   k   u      7   8   9         ^     ",
        "      \\ | /          \\ | /          \\ | /           |  ",
        "   a -  s  - d    h -  .  - l    4 -  5  - 6   < -  ,  - >",
        "      / | \\          / | \\          / | \\           |  ",
        "    z   x   c      n   j   m      1   2   3         v     ",
        "",
        "  Actions",
        "A      -  attack          D      -  get drunk",
        "M      -  magic",
        "",
        "  Misc.",
        "Enter  -  combat menu",
        "i      -  inventory",
        NULL };

    assert( help_text[(sizeof(help_text) / sizeof(char *))-1] == NULL );
    prompt( "Battle Dungeons Help", help_text );
}

void refresh_screen()
{
    int indent = max(strlen(game.pc->name)+3, 15) + 4;

    werase(game.message_win);
    werase(game.stats_win);
    mvwprintw(game.stats_win, 1, 1,      "%s",             game.pc->name);
    mvwprintw(game.stats_win, 2, 1,      "lvl/exp: %i/%i", game.pc->data.character->level, game.pc->data.character->exp );
    mvwprintw(game.stats_win, 1, indent, "hp: %i(%i)",     game.pc->data.character->hp,    game.pc->data.character->hp_max);
    mvwprintw(game.stats_win, 2, indent, "mp: %i(%i)",     game.pc->data.character->mp,    game.pc->data.character->mp_max);
    wrefresh(game.stats_win);
}

void get_input()
{
    bool key_ok;

    do {
        key_ok = TRUE;

        switch( wgetch(game.game_win) )
        {
        case 'h':
        case 'a':
        case '4':
        case KEY_LEFT:  walk(-1, 0, game.pc ); break;
        case 'l':
        case 'd':
        case '6':
        case KEY_RIGHT: walk( 1, 0, game.pc ); break;
        case 'k':
        case 'w':
        case '8':
        case KEY_UP:    walk( 0,-1, game.pc ); break;
        case 'j':
        case 'x':
        case '2':
        case KEY_DOWN:  walk( 0, 1, game.pc ); break;
        case 'y':
        case 'q':
        case '7':       walk(-1,-1, game.pc ); break;
        case 'u':
        case 'e':
        case '9':       walk( 1,-1, game.pc ); break;
        case 'n':
        case 'z':
        case '1':       walk(-1, 1, game.pc ); break;
        case 'm':
        case 'c':
        case '3':       walk( 1, 1, game.pc ); break;
        case '.':
        case 's':
        case '5':
        case ',':       /*TODO:Interact*/ break;
	
            //case 'a': attack( &game.pc, &game.enemy );   break;
            //case 'm': magic();                        break;
        case 'i':
            inventory();
            touchwin( game.game_win );
            break;
        case '\n':
            combat_menu( game.current_grid );
            touchwin( game.game_win );
            break;
        case 'H':
            help_menu();
            touchwin( game.game_win );
            break;
        case 'D':
            game.pc->data.character->status.drunk =! game.pc->data.character->status.drunk;
            break;

        default:
            key_ok = FALSE;
            break;
        }

    } while (!key_ok);
}

void enemy_round(entity_t *enemy)
{
    int vertical   = (game.pc->y - enemy->y) > 0 ? 1 : -1,
        horizontal = (game.pc->x - enemy->x) > 0 ? 1 : -1;
    walk( horizontal, vertical, enemy );
}
/* I.A.
   si el enemy está acorralado, atacar en modo berserk xD
   si el enemy está lejos, atacar a distancia o con magia
   si el enemy está cerca, atacar con mele
   si no quedan pm o pivotes, correr hacia el enemy
   si se está al borde de la muerte, huir, pero si se es acorralado... MATAR */

void main_loop()
{
    erase();
//  printw( "\nNext enemy: %s\n", enemy->name );
//  printw( "\n\n\nROUND 1:\n" );
//  usleep (20000);
//  ascii_fight(-1, -1);
//  usleep (10000);
    erase();

    generate_game_screen();
    keypad( game.game_win, TRUE );  

    draw_grid( game.current_grid );

    do
    {
        /* Drawing */

        if ( !game.pc->data.character->status.blind )
            visibility_area();

        draw_grid( game.current_grid );/*REMOVE*/

        for (int i=0; i<game.n_entities; i++)
        {
            draw_entity( game.entities[i] );
        }
        draw_entity( game.pc );

        refresh_screen();

        /* Input */
    
        get_input();

        for (int i=0; i<game.n_npcs; i++)
        {
            if (game.npcs[i]->data.character->status.alive)
            {
                if ( game.npcs[i]->data.character->aggressive )
                {
                    enemy_round( game.npcs[i] );
                }
                if ( game.npcs[i]->data.character->hp <= 0 )
                {
                    game.npcs[i]->data.character->status.alive = FALSE;
                    game.npcs[i]->tile = '&';
                }
            }
        }
    }
    while ( game.pc->data.character->hp > 0 );

    if (game.pc->data.character->hp <= 0)
    {
        printw( "You are dead." );
        printw( "GAME OVER" );
    }
}

void select_character()
{
#if 0
    pc_selection_menu_win = newwin( 15, 40, LINES*3/4-20, COLS/2-40);
    get_focus(pc_selection_menu_win);
    wattron(pc_selection_menu_win, COLOR_PAIR(3));
    mvwprintw( pc_selection_menu_win, 1, 3, "Select player" );
    wattron(pc_selection_menu_win, COLOR_PAIR(5));
    mvwprintw( pc_selection_menu_win, 3, 2, "Name   :" ); //scanf( "%c", *pc->name );
    mvwprintw( pc_selection_menu_win, 4, 2, "Race   :" ); //scanf( "%c", *pc->raza );

    wgetch(pc_selection_menu_win);
    drop_focus(pc_selection_menu_win);
#endif

    game.pc->data.character->hp = game.pc->data.character->hp_max;
    game.pc->data.character->mp = game.pc->data.character->mp_max;
}

void configure_test_enemies()
{
#if 0
    int option, y=2;
    WINDOW *pc_selection_menu_win, *enemy_selection_menu_win;
    char *menu_options[] = {
        "Summon enemy",
        "Random enemy",
        "Cancel",
        "",
    };

    assert( strlen(menu_options[(sizeof(menu_options) / sizeof(char *))-1]) == 0 );

    enemy_selection_menu_win = newwin( 10, 29, LINES/2-10, COLS/2-4);
    get_focus( enemy_selection_menu_win );
    wattron(enemy_selection_menu_win, COLOR_PAIR(3));
    mvwprintw( enemy_selection_menu_win, 1, 3, "Select enemy:" );

    option = menu( enemy_selection_menu_win, y+2, menu_options, MAIN_MENU_PAIR );
    switch( option )
    {
    case 1:
        rendezvous( game.npcs[0] );
        break;
    case 2:
        meele( game.npcs[0] );
        break;
    default:
        break;
    }

    destroy_win(pc_selection_menu_win);
    destroy_win(enemy_selection_menu_win);
#endif

    for (int i=0; i<game.n_npcs; i++)
    {
        game.npcs[i]->data.character->hp = game.npcs[i]->data.character->hp_max;
        game.npcs[i]->data.character->status.alive = TRUE;
    }
}

void new_combat()
{
    select_character();
    configure_test_enemies();

    main_loop();
}
