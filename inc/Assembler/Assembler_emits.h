#ifndef ASSEMBLER_EMITS_H
#define ASSEMBLER_EMITS_H

#include "Assembler_structs.h"

void emitSymsTable(Assembler_state* a);
void emitRelocTable(Assembler_state* a);
AsmERROR emitSectionHeader(Assembler_state* a);

#endif