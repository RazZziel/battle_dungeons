all: eval 

y.tab.c: eval.y
	yacc -d $<
	
lex.yy.c: eval.l
	lex $<
	
eval: y.tab.o lex.yy.o
	$(CC) -o $@ $^

