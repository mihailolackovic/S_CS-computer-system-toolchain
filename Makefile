ASM_SRC=
ASM_INC=
ASM_OBJ=


cleanup:	assembler
	sudo cp assembler /usr/local/bin/
	rm -f *.o *.c binASM/*.o assembler
	rm -f misc/parser.tab.c misc/parser.tab.h misc/lexer.c misc/lexer.h
	rmdir binASM

assembler:	parser.o lexer.o keywords.o main.o binASM/CLI_Parser.o
	gcc -Iinc/Assembler -Imisc main.o binASM/CLI_Parser.o parser.o lexer.o keywords.o -o assembler -lfl

main.o:	misc/lexer.h src/Assembler/main.c
	gcc -Iinc/Assembler -Imisc -c src/Assembler/main.c -o main.o

binASM:
	mkdir -p binASM

binASM/CLI_Parser.o: src/Assembler/CLI_Parser.c inc/Assembler/CLI_Parser.h | binASM
	gcc -Iinc/Assembler -c src/Assembler/CLI_Parser.c -o binASM/CLI_Parser.o

keywords.o: misc/keywords.h
	gcc -c misc/keywords.c -o keywords.o

lexer.o: misc/lexer.l misc/keywords.h parser.o misc/parser.tab.c
	flex -o lex.yy.c misc/lexer.l
	gcc -Wall -Imisc -c lex.yy.c -o lexer.o

parser.o: misc/parser.y misc/keywords.h misc/instructions.h
	bison -o misc/parser.tab.c -d misc/parser.y
	gcc -Wall -Imisc -c misc/parser.tab.c -o parser.o


clean:
	rm -f *.o *.c binASM/*.o assembler
	rm -f misc/parser.tab.c misc/parser.tab.h misc/lexer.c misc/lexer.h
	rmdir binASM