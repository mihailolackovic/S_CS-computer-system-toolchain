#ifndef ASSEMBLE_H
#define ASSEMBLE_H

#include "../../inc/Assembler/instructions.h"
#include "Assembler_functions.h"

void Assemble(Instruction* instructions, int instruction_count, FILE* output);

AsmERROR resolveFD(Assembler_state* a);
AsmERROR resolvePCRel(Assembler_state* a);

void emitSymsTable(Assembler_state* a);
void emitRelocTable(Assembler_state* a);
AsmERROR emitSectionHeader(Assembler_state* a);

AsmERROR handleEXIT(AsmERROR error, Instruction* ins);

void printInstruction(Instruction ins);
void printSectionTable(Assembler_state* a);
void printSymsTable(Assembler_state* a);
void printRelocTable(Assembler_state* a);

#endif