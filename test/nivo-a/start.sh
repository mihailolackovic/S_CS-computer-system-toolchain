ASSEMBLER=./assembler
LINKER=./linker
EMULATOR=./emulator
path=/home/ss/Desktop/Projekat/test/nivo-a/

clear

${ASSEMBLER} -o main.o ${path}main.s
${ASSEMBLER} -o math.o ${path}math.s
${ASSEMBLER} -o handler.o ${path}handler.s
${ASSEMBLER} -o isr_timer.o ${path}isr_timer.s
${ASSEMBLER} -o isr_terminal.o ${path}isr_terminal.s
${ASSEMBLER} -o isr_software.o ${path}isr_software.s
${LINKER} -hex \
  -place=my_code@0x40000000 -place=math@0xF0000000 \
  -o program.hex \
  handler.o math.o main.o isr_terminal.o isr_timer.o isr_software.o
${EMULATOR} program.hex