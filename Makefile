CC = gcc

CFLAGS = -g -Wall   \
	-Iinc/Assembler   \
	-Iinc/Linker      \
	-Iinc/Emulator    \
	-Imisc

BINASM  = binASM
BINLINK = binLINK
BINEMU  = binEMU

ASM_SRC  := $(wildcard src/Assembler/*.c)
LINK_SRC := $(wildcard src/Linker/*.c)
EMU_SRC  := $(wildcard src/Emulator/*.c)

ASM_OBJ  := $(patsubst src/Assembler/%.c,$(BINASM)/%.o,$(ASM_SRC))
LINK_OBJ := $(patsubst src/Linker/%.c,$(BINLINK)/%.o,$(LINK_SRC))
EMU_OBJ  := $(patsubst src/Emulator/%.c,$(BINEMU)/%.o,$(EMU_SRC))

all: assembler linker install clean distclean# emulator
link: linker install clean distclean
allDebug: assembler linker cleanDebug distclean# emulator

assembler: parser lexer $(ASM_OBJ)
	$(CC) $(CFLAGS) $(ASM_OBJ) parser.o lexer.o -o $@ -lfl

linker: $(LINK_OBJ)
	$(CC) $(CFLAGS) $(LINK_OBJ) -o $@

emulator: $(EMU_OBJ)
	$(CC) $(CFLAGS) $(EMU_OBJ) -o $@

$(BINASM) $(BINLINK) $(BINEMU):
	mkdir -p $@

$(BINASM)/%.o: src/Assembler/%.c | $(BINASM)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINLINK)/%.o: src/Linker/%.c | $(BINLINK)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINEMU)/%.o: src/Emulator/%.c | $(BINEMU)
	$(CC) $(CFLAGS) -c $< -o $@

parser: parser.o

parser.o: misc/parser.y
	bison -d -o misc/parser.tab.c misc/parser.y
	$(CC) $(CFLAGS) -c misc/parser.tab.c -o parser.o

lexer: lexer.o

lexer.o: misc/lexer.l parser.o
	flex -o lex.yy.c misc/lexer.l
	$(CC) $(CFLAGS) -c lex.yy.c -o lexer.o

install: assembler linker
	sudo cp assembler /usr/local/bin/
	sudo cp linker /usr/local/bin/

clean:
	rm -f $(BINASM)/*.o
	rm -f $(BINLINK)/*.o
	rm -f $(BINEMU)/*.o
	rm -f parser.o lexer.o
	rm -f assembler linker emulator
	rm -f misc/parser.tab.c misc/parser.tab.h
	rm -f lex.yy.c
	rm -f misc/lexer.h

cleanDebug:
	rm -f $(BINASM)/*.o
	rm -f $(BINLINK)/*.o
	rm -f $(BINEMU)/*.o
	rm -f parser.o lexer.o
	rm -f misc/parser.tab.c misc/parser.tab.h
	rm -f lex.yy.c
	rm -f misc/lexer.h

distclean:
	rmdir $(BINASM) 2>/dev/null || true
	rmdir $(BINLINK) 2>/dev/null || true
	rmdir $(BINEMU) 2>/dev/null || true

.PHONY: all clean distclean install parser lexer