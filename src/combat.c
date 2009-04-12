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

void attack(pj_t *pj, pj_t *enemy)
{
/* Ataca al enemigo.                                             *
 * Mediante una tirada de random sobre 20, determina si el golpe *
 * es normal, crítico o pifia.                                   *
 * La dureza del golpe es directamente proporcional a la fuerza  *
 * del pj y a la fuerza del arma, e inversamente proporcional    *
 * a la defensa del oponente.                                    */
//  boolean critical = FALSE;
    int hit;
  
    //  text = "¡Atacas al " + enemy->name + " con tu " + "cepillodientes" + "!\n";
    //  message( "You attack %s", "random enemy" );
    if (pj->playable)
        message("Atacas al %s", enemy->name);
    else
        message("El %s te ataca", pj->name);
    // TODO ver lo de los ataques con rango
    //  if ( dices( 1,20,(base_attack_bonus + modifier( pj->str )+ size mod )
    //	      < ( 10 + armor bonus + shield bonus + modifier( enemy->dex ) + enemy->size )))
    //    message("Fallas el golpe*\n");
    //  else 
    //    hit = dices( pj->arma->dado, 6, ( base atack bonus + modifier( enemy->str ) + enemy->tamanyo ));
  
    //  if (pifia)
    //    message( "%s%s se ríe de ti...\n", enemy->prefix, enemy->name );
    //  else if (critico)
    //    message( "¡¡Le asestas un golpe mortal a%s%s!!\n", enemy->prefix, enemy->name );
    hit = 0;//6;
    enemy->pg -= hit;
}

void walk (grid_t *grid, int horizontal, int vertical, pj_t *pj, pj_t *enemy)
{
    int collision;

    /* drunk characters detection */
    if (pj->status.drunk)
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
        ( pj->x + ( horizontal ) == ( grid->width ) )  ||
        ( pj->x + ( horizontal ) == -1 )               ||
        ( pj->y + ( vertical   ) == ( grid->height ) ) ||
        ( pj->y + ( vertical   ) == -1 )               ||
    
        /* collision detection with obstacles */
        ( grid_node( grid, ( pj->y + vertical), ( pj->x + horizontal ) )->solid == TRUE );
  
    /* collision detection with enemy */
    if ( ( pj->x + ( horizontal ) == enemy->x ) && ( pj->y + ( vertical ) == enemy->y ) )
    {
        attack(pj, enemy);
    }
    else if (collision==0)
    {
        draw_node(grid, pj->y, pj->x);
        pj->x += horizontal;
        pj->y += vertical;
        if (!pj->status.blind && pj->playable)
            visibility_area(grid, pj);
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

void refresh_screen(pj_t *pj, pj_t *enemy)
{
    int indent = max(strlen(pj->name)+3, 15) + 4;

    werase(game.message_win);
    werase(game.stats_win);
    mvwprintw(game.stats_win, 1, 1,      "%s",             pj->name);
    mvwprintw(game.stats_win, 2, 1,      "lvl/exp: %i/%i", pj->level, pj->exp );
    mvwprintw(game.stats_win, 1, indent, "pg: %i(%i)",     pj->pg, pj->pg_max);
    mvwprintw(game.stats_win, 2, indent, "mp: %i(%i)",     pj->mp, pj->mp_max);
    wrefresh(game.stats_win);
}

int pj_round (pj_t *pj, pj_t *enemy)
{
    bool key_ok;

    do {
        key_ok = TRUE;

        switch( wgetch(game.game_win) )
        {
        case KEY_LEFT:  walk(game.grid[0], -1, 0, pj, enemy ); break;
        case '4':       walk(game.grid[0], -1, 0, pj, enemy ); break;
        case KEY_RIGHT: walk(game.grid[0],  1, 0, pj, enemy ); break;
        case '6':       walk(game.grid[0],  1, 0, pj, enemy ); break;
        case KEY_UP:    walk(game.grid[0],  0,-1, pj, enemy ); break;
        case '8':       walk(game.grid[0],  0,-1, pj, enemy ); break;
        case KEY_DOWN:  walk(game.grid[0],  0, 1, pj, enemy ); break;
        case '2':       walk(game.grid[0],  0, 1, pj, enemy ); break;
        case '7':       walk(game.grid[0], -1,-1, pj, enemy ); break;
        case '9':       walk(game.grid[0],  1,-1, pj, enemy ); break;
        case '1':       walk(game.grid[0], -1, 1, pj, enemy ); break;
        case '3':       walk(game.grid[0],  1, 1, pj, enemy ); break;
	
        case 'a': attack( pj, enemy );             break;
        case 'm': /*magic();*/                     break;
        case 'o': /*object();*/                    break;
        case 'i':
            inventory();
            touchwin( game.game_win );
            break;
        case '\n':
            combat_menu( game.grid[0] );
            touchwin( game.game_win );
            break;
        case 'h':
            help_menu();
            touchwin( game.game_win );
            break;
        case 'd':
            pj->status.drunk=!pj->status.drunk;
            break;

        default:
            key_ok = FALSE;
            break;
        }

    } while (!key_ok);

    return FALSE; /* default = FALSE */
}

int enemy_round(pj_t *pj, pj_t *enemy)
{
    int vertical   = ((pj->y)-(enemy->y)),
        horizontal = ((pj->x)-(enemy->x));

    if (vertical>0)        vertical = 1;
    else if (vertical<0)   vertical =-1;
    else                   vertical = 0;

    if (horizontal>0)      horizontal = 1;
    else if (horizontal<0) horizontal =-1;
    else                   horizontal = 0;

    if (enemy->pg<5) {
        horizontal = -horizontal;
        vertical = -vertical;
    }

    if (grid_node(game.grid[0], pj->y+vertical, pj->x)->solid)
        vertical=0;
    if (grid_node(game.grid[0], pj->y+vertical, pj->x+horizontal)->solid)
        horizontal=0;

    walk(game.grid[0], horizontal, vertical, enemy, pj);

    return TRUE; /* default = TRUE */
}
/* I.A.
   si el enemy está acorralado, atacar en modo berserk xD
   si el enemy está lejos, atacar a distancia o con magia
   si el enemy está cerca, atacar con mele
   si no quedan pm o pivotes, correr hacia el enemy
   si se está al borde de la muerte, huir, pero si se es acorralado... MATAR */

void test_combat(pj_t *pj, pj_t *enemy)
{
    bool pj_turn;
    int round=1,
        temp_round=0;

    erase();
    printw( "\nNext enemy: %s\n", enemy->name );
    printw( "\n\n\nROUND 1:\n" );
//  usleep (20000);
//  ascii_fight(-1, -1);
//  usleep (10000);
    erase();

    generate_game_screen();
    //new_grid( &game.grid[0], LINES-7, COLS );
    //create_first_combat_grid( game.grid[0], pj, enemy );
    keypad( game.game_win, TRUE );  

    draw_grid( game.grid[0], pj, enemy);
    //  pj_turn = ( dices(1, 20, modifier(pj->dex)) > dices(1, 20, modifier(enemy->dex)) );
    pj_turn = TRUE; //

    do
    {    
        refresh_screen(pj, enemy); 
    
        if (pj_turn==1)
        {

            if (pj->status.dead==0)
                pj_turn = pj_round( pj, enemy );
            else
                pj_turn = FALSE;

        }
        else
        {

            if (enemy->status.dead==0)
                pj_turn = enemy_round( pj, enemy );
            else
                pj_turn = TRUE;

        }
        draw_pj(game.grid[0], pj);
        draw_pj(game.grid[0], enemy);
    
        if (enemy->pg<=0 && enemy->status.dead==0) {
            enemy->status.dead = 1;
            enemy->avatar = '&';
            draw_pj(game.grid[0], enemy);
            temp_round = round;
        }
    
        if(enemy->status.dead==1 && round==(temp_round+10)) {
            enemy->status.dead = 0;
            enemy->status.undead = 1;
            enemy->status.drunk = 1;
            enemy->pg = 1000;
        }

        round++;
    }
    while ( ( pj->pg > 0 ));
  
  
    if (pj->pg <= 0)
    {
        printw( "You are dead." );
        printw( "GAME OVER" );
    }
    else if (enemy->pg <= 0)
    {
        printw( "%s%s bites the dust.", enemy->prefix, enemy->name );
        printw( "¡¡You win!!" );
    }
  
}

void new_combat()
{
    WINDOW *pj_selection_menu_win, *enemy_selection_menu_win;
    pj_t *pj, *enemy;
    char *menu_options[] = {
        "Summon enemy",
        "Random enemy",
        "Cancel",
        "",
    };
    int option, y=2;

    assert( strlen(menu_options[(sizeof(menu_options) / sizeof(char *))-1]) == 0 );

    pj = malloc(sizeof(pj_t));
#if 0
    pj_selection_menu_win = newwin( 15, 40, LINES*3/4-20, COLS/2-40);
    get_focus(pj_selection_menu_win);
    wattron(pj_selection_menu_win, COLOR_PAIR(30));
    mvwprintw( pj_selection_menu_win, 1, 3, "Select player" );
    wattron(pj_selection_menu_win, COLOR_PAIR(50));
    mvwprintw( pj_selection_menu_win, 3, 2, "Name   :" ); //scanf( "%c", *pj->name );
    mvwprintw( pj_selection_menu_win, 4, 2, "Race   :" ); //scanf( "%c", *pj->raza );

    wgetch(pj_selection_menu_win);
    drop_focus(pj_selection_menu_win);
#endif
    enemy = malloc(sizeof(pj_t));
#if 0
    enemy_selection_menu_win = newwin( 10, 29, LINES/2-10, COLS/2-4);
    get_focus( enemy_selection_menu_win );
    wattron(enemy_selection_menu_win, COLOR_PAIR(30)); 
    mvwprintw( enemy_selection_menu_win, 1, 3, "Select enemy:" );
#endif

    pj->name = "Raziel";
    pj->color = 10;
    pj->avatar = '@';
    pj->pg = 30; pj->pg_max = 30;
    pj->mp = 13; pj->mp_max = 13;
    pj->level = pj->exp = 0;
    pj->playable = pj->status.dead = pj->status.drunk = pj->status.blind = 0;
    enemy->name = "Zarovich";
    enemy->color = 40;
    enemy->avatar = 'Z';
    enemy->pg = 10;
    enemy->playable = enemy->status.dead = enemy->status.drunk = enemy->status.blind = 0;
    pj->x = pj->y = 1;
    enemy->x = enemy->y = 5;
    test_combat( pj, enemy );
#if 0
    option = menu( enemy_selection_menu_win, y+2, menu_options, MAIN_MENU_PAIR );
    switch( option )
    {
    case 1:
        //buscar_enemigo( enemy );
        test_combat( pj, enemy );
        break;
    case 2:
        //enemigo_aleatorio( enemy );
        test_combat( enemy, pj );
        break;
    default:
        break;
    }

    destroy_win(pj_selection_menu_win);
    destroy_win(enemy_selection_menu_win);
#endif
}
