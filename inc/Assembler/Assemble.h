#ifndef ASSEMBLE_H
#define ASSEMBLE_H

#include "../../inc/Assembler/instructions.h"
#include "Assembler_functions.h"
#include "Assembler_emits.h"
#include "Assembler_debug.h"

void Assemble(Instruction* instructions, int instruction_count, FILE* output);

AsmERROR resolveFD(Assembler_state* a);
AsmERROR resolveReloc(Assembler_state* a);

AsmERROR handleEXIT(AsmERROR error, Instruction* ins);

#endif