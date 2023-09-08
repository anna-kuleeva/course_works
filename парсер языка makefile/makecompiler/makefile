clean:
	rm -f *.o *.yy.c *.tab.* maker

all: clean
	bison -dt makeg.y
	flex terminals.l
	gcc -w main.c makeg.tab.c lex.yy.c -o maker
	
