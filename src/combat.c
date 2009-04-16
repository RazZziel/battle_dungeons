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
#include "colors.h"
#include "character.h"
#include "global.h"
#include "scripting.h"

extern game_engine_t game;

void attack(pc_t *attacker, pc_t *victim)
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
    if (attacker->playable)
        message("Atacas al %s", victim->name);
    else
        message("El %s te ataca", attacker->name);
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
    hit = 0;//6;
    victim->hp -= hit;
}

void walk(int horizontal, int vertical, pc_t *pc)
{
    int collision;

    /* drunk characters detection */
    if (pc->status.drunk)
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
  
    collision =  
        /* collision detection with grid */
        ( (pc->x + horizontal) == game.current_grid->width )  ||
        ( (pc->x + horizontal) == -1 )                        ||
        ( (pc->y + vertical)   == game.current_grid->height ) ||
        ( (pc->y + vertical)   == -1 )                        ||
    
        /* collision detection with obstacles */
        ( grid_node( game.current_grid,( pc->y + vertical),
                     ( pc->x + horizontal ) )->solid == TRUE );
  
    /* collision detection with enemy */
    for (int i=0; i<game.n_npcs; i++)
    {
        if ( ( pc->x + ( horizontal ) == game.npcs[i].x ) &&
             ( pc->y + ( vertical ) == game.npcs[i].y ) )
        {
            attack(pc, &game.npcs[i]);
        }
        else if (collision==0)
        {
            draw_node(pc->y, pc->x);
            pc->x += horizontal;
            pc->y += vertical;
        }
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

    switch( menu(combat_menu_win, 1, menu_options, MAIN_MENU_PAIR) )
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
#define MENU_WIN_HEIGHT LINES-10
#define MENU_WIN_WIDTH  COLS-10
    WINDOW * help_win;
    int i;

    help_win = newwin(MENU_WIN_HEIGHT, MENU_WIN_WIDTH, 5, 5);
    get_focus(help_win);
    mvwprintw(help_win, i=1, (MENU_WIN_WIDTH-20)/2, "Battle Dungeons Help");
    i++;
    mvwprintw(help_win, ++i, 5, "Movement");
    mvwprintw(help_win, ++i, 3, "                           N");
    mvwprintw(help_win, ++i, 3, "      7   8   9          W + E");
    mvwprintw(help_win, ++i, 3, "        \\ | /              S");
    mvwprintw(help_win, ++i, 3, "{   4 --  O  -- 6   }");
    mvwprintw(help_win, ++i, 3, "        / | \\");
    mvwprintw(help_win, ++i, 3, "      1   2   3");
    i++;
    mvwprintw(help_win, ++i, 5, "Actions");
    mvwprintw(help_win, ++i, 3, "a      -  attack");
    mvwprintw(help_win,   i, (MENU_WIN_WIDTH-16)/2, "d      -  get drunk");
    mvwprintw(help_win, ++i, 3, "m      -  magic");
    i++;
    mvwprintw(help_win, ++i, 5, "Misc.");
    mvwprintw(help_win, ++i, 3, "Enter  -  combat menu");
    mvwprintw(help_win, ++i, 3, "i      -  inventory");

    mvwprintw(help_win, MENU_WIN_HEIGHT-2, MENU_WIN_WIDTH-35, "Press any key...");
    wgetch(help_win);
    destroy_win(help_win);
}

void refresh_screen()
{
    int indent = max(strlen(game.pc.name)+3, 15) + 4;

    werase(game.message_win);
    werase(game.stats_win);
    mvwprintw(game.stats_win, 1, 1,      "%s",             game.pc.name);
    mvwprintw(game.stats_win, 2, 1,      "lvl/exp: %i/%i", game.pc.level, game.pc.exp );
    mvwprintw(game.stats_win, 1, indent, "hp: %i(%i)",     game.pc.hp,    game.pc.hp_max);
    mvwprintw(game.stats_win, 2, indent, "mp: %i(%i)",     game.pc.mp,    game.pc.mp_max);
    wrefresh(game.stats_win);
}

void get_input()
{
    bool key_ok;

    do {
        key_ok = TRUE;

        switch( wgetch(game.game_win) )
        {
        case KEY_LEFT:  walk(-1, 0, &game.pc ); break;
        case '4':       walk(-1, 0, &game.pc ); break;
        case KEY_RIGHT: walk( 1, 0, &game.pc ); break;
        case '6':       walk( 1, 0, &game.pc ); break;
        case KEY_UP:    walk( 0,-1, &game.pc ); break;
        case '8':       walk( 0,-1, &game.pc ); break;
        case KEY_DOWN:  walk( 0, 1, &game.pc ); break;
        case '2':       walk( 0, 1, &game.pc ); break;
        case '7':       walk(-1,-1, &game.pc ); break;
        case '9':       walk( 1,-1, &game.pc ); break;
        case '1':       walk(-1, 1, &game.pc ); break;
        case '3':       walk( 1, 1, &game.pc ); break;
	
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
        case 'h':
            help_menu();
            touchwin( game.game_win );
            break;
        case 'd':
            game.pc.status.drunk =! game.pc.status.drunk;
            break;

        default:
            key_ok = FALSE;
            break;
        }

    } while (!key_ok);
}

void enemy_round(pc_t *enemy)
{
    int vertical   = ((game.pc.y)-(enemy->y)),
        horizontal = ((game.pc.x)-(enemy->x));

    if (vertical>0)        vertical = 1;
    else if (vertical<0)   vertical =-1;
    else                   vertical = 0;

    if (horizontal>0)      horizontal = 1;
    else if (horizontal<0) horizontal =-1;
    else                   horizontal = 0;

    if (enemy->hp<5)
    {
        horizontal = -horizontal;
        vertical = -vertical;
    }

    if ( (game.pc.y+vertical < 0) ||
         (game.pc.y+vertical >= game.current_grid->height) ||
         (grid_node(game.current_grid, game.pc.y+vertical, game.pc.x)->solid) )
    {
        vertical = 0;
    }
    if ( (game.pc.y+horizontal < 0) ||
         (game.pc.y+horizontal >= game.current_grid->width) ||
         (grid_node(game.current_grid, game.pc.y, game.pc.x+horizontal)->solid) )
    {
        horizontal = 0;
    }

    walk(horizontal, vertical, enemy);
}
/* I.A.
   si el enemy está acorralado, atacar en modo berserk xD
   si el enemy está lejos, atacar a distancia o con magia
   si el enemy está cerca, atacar con mele
   si no quedan pm o pivotes, correr hacia el enemy
   si se está al borde de la muerte, huir, pero si se es acorralado... MATAR */

void main_loop()
{
    int round=1,
        temp_round=0;

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
    draw_pc( &game.pc );
    for (int i=0; i<game.n_npcs; i++)
        draw_pc( &game.npcs[i] );

    do
    {
        refresh_screen();
    
        get_input();

        for (int i=0; i<game.n_npcs; i++)
        {
            enemy_round( &game.npcs[i] );
    
            if ( (game.npcs[i].hp <= 0) &&
                 (game.npcs[i].status.alive) )
            {
                game.npcs[i].status.alive = 0;
                game.npcs[i].avatar = '&';
                temp_round = round;
            }
    
            if( (game.npcs[i].status.alive) &&
                (round == (temp_round+10)) )
            {
                game.npcs[i].status.alive = 1;
                game.npcs[i].status.drunk = 1;
                game.npcs[i].hp = 1000;
            }
        }


        /* Drawing */

        if ( !game.pc.status.blind )
            visibility_area();

        draw_pc( &game.pc );

        for (int i=0; i<game.n_npcs; i++)
            draw_pc( &game.npcs[i] );


        round++;
    }
    while ( game.pc.hp > 0 );

    if (game.pc.hp <= 0)
    {
        printw( "You are dead." );
        printw( "GAME OVER" );
    }
}

void new_combat()
{
    WINDOW *pc_selection_menu_win, *enemy_selection_menu_win;
    char *menu_options[] = {
        "Summon enemy",
        "Random enemy",
        "Cancel",
        "",
    };
    int option, y=2;

    assert( strlen(menu_options[(sizeof(menu_options) / sizeof(char *))-1]) == 0 );

#if 0
    pc_selection_menu_win = newwin( 15, 40, LINES*3/4-20, COLS/2-40);
    get_focus(pc_selection_menu_win);
    wattron(pc_selection_menu_win, COLOR_PAIR(30));
    mvwprintw( pc_selection_menu_win, 1, 3, "Select player" );
    wattron(pc_selection_menu_win, COLOR_PAIR(50));
    mvwprintw( pc_selection_menu_win, 3, 2, "Name   :" ); //scanf( "%c", *pc->name );
    mvwprintw( pc_selection_menu_win, 4, 2, "Race   :" ); //scanf( "%c", *pc->raza );

    wgetch(pc_selection_menu_win);
    drop_focus(pc_selection_menu_win);
#endif
    game.n_npcs = 1;
    game.npcs = malloc( sizeof(pc_t) * game.n_npcs );
#if 0
    enemy_selection_menu_win = newwin( 10, 29, LINES/2-10, COLS/2-4);
    get_focus( enemy_selection_menu_win );
    wattron(enemy_selection_menu_win, COLOR_PAIR(30)); 
    mvwprintw( enemy_selection_menu_win, 1, 3, "Select enemy:" );
#endif

    game.pc.name = "Raziel";
    game.pc.color = 10;
    game.pc.avatar = '@';
    game.pc.hp = 30; game.pc.hp_max = 30;
    game.pc.mp = 13; game.pc.mp_max = 13;
    game.pc.level = game.pc.exp = 0;
    game.pc.status.alive =     \
        game.pc.status.drunk = \
        game.pc.status.blind = 0;
    game.pc.playable = 1;
    game.pc.range_sight = 5;

    game.npcs[0].name = "Zarovich";
    game.npcs[0].color = 40;
    game.npcs[0].avatar = 'Z';
    game.npcs[0].hp = 10;
    game.npcs[0].playable = \
        game.npcs[0].status.alive = \
        game.npcs[0].status.drunk = \
        game.npcs[0].status.blind = 0;
    game.pc.x = game.pc.y = 1;
    game.npcs[0].x = game.npcs[0].y = 5;

    main_loop();

#if 0
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
}
