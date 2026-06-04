#ifndef ASSEMBLE_H
#define ASSEMBLE_H

#include "../../misc/instructions.h"
#include "Assembler_functions.h"

void Assemble(Instruction* instructions, int instruction_count, FILE* output);

void printInstruction(Instruction ins);
void printSymsTable(Assembler_state* a);

#endif