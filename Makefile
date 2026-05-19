clean:	program
	rm -f *.o *.c

program:	lexer.o keywords.o
	gcc lexer.o keywords.o -o program -lfl

keywords.o: misc/keywords.h
	gcc -c misc/keywords.c -o keywords.o

lexer.o: misc/lexer.l misc/keywords.h
	flex -o lex.yy.c misc/lexer.l
	gcc -Wall -Imisc -c lex.yy.c -o lexer.o
