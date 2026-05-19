ASM_SRC=
ASM_INC=
ASM_OBJ=


clean:	assembler
	rm -f *.o *.c binASM/*.o
	rmdir binASM
	sudo cp assembler /usr/local/bin/

assembler:	lexer.o keywords.o main.o binASM/CLI_Parser.o
	gcc -Iinc/Assembler -Imisc main.o binASM/CLI_Parser.o lexer.o keywords.o -o assembler -lfl

main.o:	misc/lexer.h src/Assembler/main.c
	gcc -Iinc/Assembler -Imisc -c src/Assembler/main.c -o main.o

binASM:
	mkdir -p binASM

binASM/CLI_Parser.o: src/Assembler/CLI_Parser.c inc/Assembler/CLI_Parser.h | binASM
	gcc -Iinc/Assembler -c src/Assembler/CLI_Parser.c -o binASM/CLI_Parser.o

keywords.o: misc/keywords.h
	gcc -c misc/keywords.c -o keywords.o

lexer.o: misc/lexer.l misc/keywords.h
	flex -o lex.yy.c misc/lexer.l
	gcc -Wall -Imisc -c lex.yy.c -o lexer.o
