CC=gcc
LEX=flex
YACC=bison
OUT=eval
LDFLAGS=-ly -lfl

all: ${OUT}

run: all
	-./${OUT} < rpg/maps.rpg
	-./${OUT} < rpg/entities.rpg
	-./${OUT} < rpg/materials.rpg


${OUT}: y.tab.c lex.yy.c
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

y.tab.c: eval.y
	$(YACC) -t -y -d $< -o $@

lex.yy.c: eval.l
	$(LEX) $<


clean:
	-rm *~ ${OUT} *.o y.tab.c lex.yy.c y.tab.h
