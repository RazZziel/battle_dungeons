bin_PROGRAMS = battle_dungeons
battle_dungeons_SOURCES = main.c ui.c menus.c ascii_art.c story.c \
                          errors.c color.c grid.c combat.c character.c dices.c \
                          y.tab.c lex.yy.c inventory.c parser.c interpreter.c \
                          symtable.c #enemies.c
battle_dungeons_LDADD = -lncurses -lm -lfl -ly #-lpopt
AM_CFLAGS = -Wall -Werror -std=c99 -fgnu89-inline -D_GNU_SOURCE -I../include $(all_includes)
AM_LDFLAGS = -rdynamic
# -rdynamic = meaningful stack traces

y.tab.c y.tab.h: parser.y
	$(YACC) --report=all -y -d $< -o $@
#	PIG DISGUSTING HACK (el bison de la fic no entiende de %code requires)
	echo '#include "interpreter.h"' > __tmp__
	cat y.tab.h >> __tmp__
	mv __tmp__ y.tab.h

lex.yy.c lex.yy.h: parser.l
	$(LEX) --header-file=lex.yy.h $<

clean-generic:
	-rm -f y.tab.c y.tab.h lex.yy.c lex.yy.h bleh.txt

check: all
	valgrind battle_dungeons 2> bleh.txt; cat bleh.txt
