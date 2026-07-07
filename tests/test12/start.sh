ASSEMBLER=./assembler

${ASSEMBLER} -o main.o tests/test12/main.s
${ASSEMBLER} -o main1.o tests/test12/main1.s
${ASSEMBLER} -o main2.o tests/test12/main2.s
${ASSEMBLER} -o main3.o tests/test12/main3.s

./linker -relocatable -o prog.o main3.o main.o main2.o main1.o

./linker -hex -o main.hex prog.o

./emulator main.hex