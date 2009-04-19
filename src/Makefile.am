bin_PROGRAMS = battle_dungeons
battle_dungeons_SOURCES = main.c windows.c menus.c ascii_art.c story.c \
                          errors.c color.c grid.c combat.c character.c dices.c \
                          y.tab.c lex.yy.c scripting.c
                          #enemies.c objetos.c
battle_dungeons_LDADD = -lncurses -lpopt -lm -lfl -ly
AM_CFLAGS = -Wall -std=c99 -D_GNU_SOURCE -I../include $(all_includes)

y.tab.c y.tab.h: scripting.y
	$(YACC) -y -d $< -o $@

lex.yy.c lex.yy.h: scripting.l
	$(LEX) --header-file=lex.yy.h $<

clean-generic:
	-rm -f y.tab.c y.tab.h lex.yy.c lex.yy.h bleh.txt

check: all
	valgrind battle_dungeons 2> bleh.txt; cat bleh.txt
