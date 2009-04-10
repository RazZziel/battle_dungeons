bin_PROGRAMS = battle_dungeons
battle_dungeons_SOURCES = main.c windows.c menus.c ascii_art.c story.c \
                          errors.c colors.c grid.c combat.c character.c dices.c \
                          y.tab.c lex.yy.c scripting.c
                          #enemies.c objetos.c
battle_dungeons_LDADD = -lncurses -lpopt -lm -lfl -ly
AM_CFLAGS = -g3 -Wall -I../include $(all_includes)

y.tab.c y.tab.h: scripting.y
	$(YACC) -y -d $< -o $@

lex.yy.c lex.yy.h: scripting.l
	$(LEX) --header-file=lex.yy.h $<

clean-generic:
	-rm -f y.tab.c y.tab.h lex.yy.c lex.yy.h

CC = grc -es gcc
