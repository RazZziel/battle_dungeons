#include <ncurses.h>
#include <unistd.h>
#include <string.h> // strlen

#include "battle.h"
#include "windows.h"
#include "menus.h"

extern game_engine_t game;

void move_selection (WINDOW *window, int *option, int direction, int left, int right, int pair)
{
    int y = getcury(window);
    mvwchgat( window, y, left, (right-left), A_NORMAL, pair, NULL );
    mvwprintw( window, y, left,  "  " );
    mvwprintw( window, y, right, "  " );

    mvwchgat( window, y + direction*2, left, (right-left), A_BOLD, pair, NULL );
    mvwprintw( window, y + direction*2, left,  "<|" );
    mvwprintw( window, y + direction*2, right, "|>" );
    *option += direction;
}

int menu(WINDOW *menu_win, int menu_y, char *options[], int color_pair)
{
    int n_options = 0;
    int option = 1,
        key, i, y = menu_y, x,
        menu_width, longer_option = 0;

    while (strlen(options[n_options]))
    {
        if (strlen(options[n_options]) > strlen(options[longer_option]))
        {
            longer_option = n_options;
        }
        n_options++;
    };
  
    menu_width = getmaxx(menu_win);
  
    wattron(menu_win, COLOR_PAIR(color_pair));
    wattron(menu_win, A_BOLD);
    x = (menu_width-strlen(options[0]))/2;
    mvwprintw( menu_win, y, x, "%s", options[0] );
  
    wattroff(menu_win, A_BOLD);
    for ( i=1; i<n_options; i++ )
    {
        x = (menu_width-strlen(options[i]))/2;
        mvwprintw( menu_win, y+=2, x, "%s", options[i] );
    }
  
#define LEFT ((menu_width-strlen(options[longer_option]))/2 - 3)
#define RIGHT (LEFT + strlen(options[longer_option]) + 4)
  
    mvwprintw( menu_win, menu_y, LEFT,  "<|" );
    mvwprintw( menu_win, menu_y, RIGHT, "|>" );
  
    wrefresh(menu_win);
  
    keypad(menu_win, TRUE);
    noecho();
    cbreak();
  
    do {
        key = wgetch(menu_win);
        switch( key )
        {
        case KEY_DOWN:
            move_selection( menu_win, &option,
                            ( (option < n_options)
                              ? 1
                              : -n_options+1 ),
                            LEFT, RIGHT, color_pair );
            break;
        case KEY_UP:
            move_selection( menu_win, &option,
                            ( (option > 1)
                              ? -1
                              : n_options-1 ),
                            LEFT, RIGHT, color_pair );
            break;
        default:
            break;
        }
        wrefresh( menu_win );
    } while( key != '\n' );

    return option;
}

/* ***
 * ***********************************************************
 *                                                         ***/

void move_page(WINDOW *window, int *option, int direction, int *temp_len, char *pages[], int pages_init)
{
    int i;

    mvwchgat( window, 1, *temp_len, strlen(pages[(*option)*2+1]), A_NORMAL, 0, NULL );

    if (direction==-2)
    {
        *temp_len = pages_init + strlen(pages[0]);
    }
    else if (direction==2)
    {
        for (i=1; i<5; i++) *temp_len += strlen(pages[i]);
    }
    else
    {
        (*temp_len) += (direction) * ( strlen(pages[(*option)*2+1+direction]) +
                                       strlen(pages[(*option)*2+direction]) );
    }

    *option += direction;

    mvwchgat( window, 1, *temp_len, strlen(pages[(*option)*2+1]), A_BOLD, 0, NULL );
    touchline( window, 1, 1 );

}

void inventory()
{
#define INVENTORY_WIN_HEIGHT (LINES-7)-4
#define INVENTORY_WIN_WIDTH 55

    WINDOW * inventory_win;
    char * inventory_pages[] = {
        "o===[====={", "Weapons", "}==={", "Armor", "}==={", "Objects", "}======>",
    };
    int option=0,
        n_pages=(sizeof(inventory_pages)/sizeof(char *)),
        total_len=0,
        temp_len,
        pages_init,
        key,
        i;  
    WINDOW * page_win[n_pages];

    inventory_win = newwin(INVENTORY_WIN_HEIGHT, INVENTORY_WIN_WIDTH, 0, COLS-INVENTORY_WIN_WIDTH-3);  
    page_win[0] = newwin(INVENTORY_WIN_HEIGHT-2, INVENTORY_WIN_WIDTH-2, 2, COLS-INVENTORY_WIN_WIDTH-2);
    for (i=1; i<n_pages; i++)
        page_win[i] = newwin(INVENTORY_WIN_HEIGHT-2, INVENTORY_WIN_WIDTH-2, 7, COLS-INVENTORY_WIN_WIDTH-2);
    for (i=0; i<n_pages; i++)
        total_len += strlen(inventory_pages[i]);
    temp_len = pages_init = (INVENTORY_WIN_WIDTH-total_len)/2;

    wmove(inventory_win, 1, pages_init);
    for (i=0; i<n_pages; i++)
        wprintw(inventory_win, "%s", inventory_pages[i]);
  
    temp_len = pages_init +  strlen(inventory_pages[0]);
    mvwchgat( inventory_win, 1, pages_init+1, 3, A_NORMAL, 30, NULL );
    mvwchgat( inventory_win, 1, pages_init+4, 1, A_BOLD, 10, NULL );
    mvwchgat( inventory_win, 1, temp_len, strlen(inventory_pages[1]), A_BOLD, 0, NULL );

    mvwprintw(page_win[0], 3, 3, "Weapons~");
    mvwprintw(page_win[1], 3, 3, "Armour~");
    mvwprintw(page_win[2], 3, 3, "Objects~");

    get_focus(inventory_win);
    for (i=2; i<6; i++)
    {
        usleep(30000);
        mvwin(inventory_win, i, COLS-INVENTORY_WIN_WIDTH-3);
        mvwin(page_win[0], i+2, COLS-INVENTORY_WIN_WIDTH-2);
    
        touchwin(game.message_win);
        wrefresh(game.message_win);

        touchline(game.game_win, 0, i-3);
        wrefresh(game.game_win);

        wrefresh(inventory_win);
        wrefresh(page_win[0]);
    }

    keypad(inventory_win, TRUE);
    noecho();
    cbreak();
    do {
        wrefresh( inventory_win );
        touchwin( page_win[option] );
        wrefresh( page_win[option] );
        key = wgetch(inventory_win);
        switch( key )
        {
        case KEY_LEFT:
            if (option > 0) move_page(inventory_win, &option, -1, &temp_len, inventory_pages, pages_init);
            else move_page(inventory_win, &option,  2, &temp_len, inventory_pages, pages_init);
            break;
        case KEY_RIGHT:
            if (option < 2) move_page(inventory_win, &option,  1, &temp_len, inventory_pages, pages_init);
            else move_page(inventory_win, &option, -2, &temp_len, inventory_pages, pages_init);
            break;
        default: break;
        }
    } while( key != 'i' );

    delwin(inventory_win);
}

