ASSEMBLER=./assembler
${ASSEMBLER} -o main.o tests/test13/main.s
${ASSEMBLER} -o main1.o tests/test13/main1.s
${ASSEMBLER} -o main2.o tests/test13/main2.s
${ASSEMBLER} -o main3.o tests/test13/main3.s
./linker -hex -o main.hex main1.o main.o main3.o main2.o
./emulator main.hex