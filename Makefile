CC = gcc
CXX = g++

CFLAGS = -g -w      \
	-Iinc/Assembler   \
	-Iinc/Linker      \
	-Iinc/Emulator    \
	-Iinc/common      \
	-Imisc						\
	-pthread

BINASM = binASM
BINLINK = binLINK
BINEMU = binEMU
BINCOMMON = binCOMMON

COMMON_SRC := $(wildcard src/common/*.c)
COMMON_OBJ := $(patsubst src/common/%.c,binCOMMON/%.o,$(COMMON_SRC))

ASM_SRC  := $(wildcard src/Assembler/*.c)
LINK_SRC := $(wildcard src/Linker/*.c)
EMU_SRC  := $(wildcard src/Emulator/*.cpp)

ASM_OBJ  := $(patsubst src/Assembler/%.c,$(BINASM)/%.o,$(ASM_SRC))
LINK_OBJ := $(patsubst src/Linker/%.c,$(BINLINK)/%.o,$(LINK_SRC))
EMU_OBJ  := $(patsubst src/Emulator/%.cpp,$(BINEMU)/%.o,$(EMU_SRC))

all: S_Obj_dump assembler linker emulator install clean distclean
allDebug: S_Obj_dump assembler linker emulator cleanDebug distclean

S_Obj_dump: $(COMMON_OBJ) src/S_Obj_dump.c
	$(CC) $(CFLAGS) -Iinc $(COMMON_OBJ) src/S_Obj_dump.c -o $@

assembler: parser lexer $(COMMON_OBJ) $(ASM_OBJ)
	$(CC) $(CFLAGS) $(COMMON_OBJ) $(ASM_OBJ) parser.o lexer.o -o $@ -lfl

linker: $(LINK_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) $(COMMON_OBJ) $(LINK_OBJ) -o $@

emulator: $(EMU_OBJ)
	$(CXX) $(CFLAGS) $(EMU_OBJ) -o $@

$(BINASM) $(BINLINK) $(BINEMU) $(BINCOMMON):
	mkdir -p $@

$(BINCOMMON)/%.o: src/common/%.c | $(BINCOMMON)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINASM)/%.o: src/Assembler/%.c | $(BINASM)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINLINK)/%.o: src/Linker/%.c | $(BINLINK)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINEMU)/%.o: src/Emulator/%.cpp | $(BINEMU)
	$(CXX) $(CFLAGS) -c $< -o $@

parser: parser.o

parser.o: misc/parser.y
	bison -d -o misc/parser.tab.c misc/parser.y
	$(CC) $(CFLAGS) -c misc/parser.tab.c -o parser.o

lexer: lexer.o

lexer.o: misc/lexer.l parser.o
	flex -o lex.yy.c misc/lexer.l
	$(CC) $(CFLAGS) -c lex.yy.c -o lexer.o

install: assembler linker emulator S_Obj_dump
	sudo cp assembler /usr/local/bin/
	sudo cp linker /usr/local/bin/
	sudo cp emulator /usr/local/bin/
	sudo cp S_Obj_dump /usr/local/bin/

clean:  cleanDebug cleanFiles
	rm -f assembler linker emulator S_Obj_dump

cleanDebug:
	rm -f $(BINASM)/*.o
	rm -f $(BINLINK)/*.o
	rm -f $(BINEMU)/*.o
	rm -f $(BINCOMMON)/*.o
	rm -f parser.o lexer.o
	rm -f misc/parser.tab.c misc/parser.tab.h
	rm -f lex.yy.c
	rm -f misc/lexer.h

distclean:
	rmdir $(BINASM) 2>/dev/null || true
	rmdir $(BINLINK) 2>/dev/null || true
	rmdir $(BINEMU) 2>/dev/null || true
	rmdir $(BINCOMMON) 2>/dev/null || true

cleanFiles:
	rm -f *.o
	rm -f *.hex
	rm -f *.txt
	rm -f *.s
	rm -f *.sh

.PHONY: all clean cleanFiles distclean install parser lexer link allDebug cleanDebug
