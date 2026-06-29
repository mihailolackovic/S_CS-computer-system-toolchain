ASSEMBLER=assembler
LINKER=linker
EMULATOR=emulator
path=/home/ss/Desktop/Projekat/test/nivo-b/

clear

${ASSEMBLER} -o main.o ${path}main.s
${ASSEMBLER} -o handler.o ${path}handler.s
${ASSEMBLER} -o isr_terminal.o ${path}isr_terminal.s
${ASSEMBLER} -o isr_timer.o ${path}isr_timer.s
${LINKER} -hex \
  -place=my_code@0x40000000 \
  -o program.hex \
  main.o isr_terminal.o isr_timer.o handler.o
${EMULATOR} program.hex