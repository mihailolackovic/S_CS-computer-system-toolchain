#ifndef ASSEMBLE_H
#define ASSEMBLE_H

#include "../../misc/instructions.h"
#include "Assembler_functions.h"

void Assemble(Instruction* instructions, int instruction_count, FILE* output);

AsmERROR handleEXIT(AsmERROR error);

void printInstruction(Instruction ins);
void printSectionTable(Assembler_state* a);
void printSymsTable(Assembler_state* a);

#endif