ASM_OBJ = binASM/CLI_Parser.o parser.o lexer.o keywords.o instructions.o Assemble.o main.o
CFLAGS = -g -Wall -Iinc/Assembler -Imisc


cleanup:	assembler
	sudo cp assembler /usr/local/bin/
	rm -f *.o *.c binASM/*.o assembler
	rm -f misc/parser.tab.c misc/parser.tab.h misc/lexer.c misc/lexer.h
	rmdir binASM

assembler:	$(ASM_OBJ)
	gcc $(CFLAGS) $(ASM_OBJ) -o assembler -lfl

main.o:	misc/parser.o src/Assembler/main.c
	gcc $(CFLAGS) -c src/Assembler/main.c -o main.o

Assemble.o: src/Assembler/Assemble.c inc/Assembler/Assemble.h
	gcc $(CFLAGS) -c src/Assembler/Assemble.c -o Assemble.o

binASM:
	mkdir -p binASM

binASM/CLI_Parser.o: src/Assembler/CLI_Parser.c inc/Assembler/CLI_Parser.h | binASM
	gcc $(CFLAGS) -c src/Assembler/CLI_Parser.c -o binASM/CLI_Parser.o

instructions.o: misc/instructions.h
	gcc $(CFLAGS) -c misc/instructions.c -o instructions.o

keywords.o: misc/keywords.h
	gcc $(CFLAGS) -c misc/keywords.c -o keywords.o

lexer.o: misc/lexer.l misc/keywords.h parser.o misc/parser.tab.c
	flex -o lex.yy.c misc/lexer.l
	gcc $(CFLAGS) -c lex.yy.c -o lexer.o

parser.o: misc/parser.y misc/keywords.h misc/instructions.h
	bison -o misc/parser.tab.c -d misc/parser.y
	gcc $(CFLAGS) -c misc/parser.tab.c -o parser.o


clean:
	rm -f *.o *.c binASM/*.o assembler
	rm -f misc/parser.tab.c misc/parser.tab.h misc/lexer.c misc/lexer.h
	rmdir binASM