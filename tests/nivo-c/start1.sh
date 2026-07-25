ASSEMBLER=./assembler
LINKER=./linker
EMULATOR=./emulator
path=./tests/nivo-c/

clear

${ASSEMBLER} -o main.o ${path}main.s
${ASSEMBLER} -o handler.o ${path}handler.s
${ASSEMBLER} -o isr_terminal.o ${path}isr_terminal.s
${ASSEMBLER} -o isr_timer.o ${path}isr_timer.s
${LINKER} -relocatable \
  -place=code@0x40000000 \
  -o program1.o \
  main.o isr_terminal.o isr_timer.o handler.o
${LINKER} -hex \
  -place=code@0x40000000 \
  -o program.hex \
  program1.o
${EMULATOR} program.hex