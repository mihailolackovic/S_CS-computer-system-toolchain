#ifndef ASSEMBLER_DEBUG_H
#define ASSEMBLER_DEBUG_H

#include "Assembler_structs.h"

void printInstruction(Instruction ins);
void printSectionTable(Assembler_state* a);
void printSymsTable(Assembler_state* a);
void printRelocTable(Assembler_state* a);

#endif