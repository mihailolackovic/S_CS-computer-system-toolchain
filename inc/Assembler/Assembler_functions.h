#ifndef ASSEMBLER_FUNCTIONS_H
#define ASSEMBLER_FUNCTIONS_H

#include "../../inc/Assembler/emit.h"
#include "Assembler_structs.h"
#include <stdlib.h>

Functions lookup_function(char* function);

AsmERROR labelSymbol(char* label, Assembler_state* a);

AsmERROR execute_function(Functions function, Operand* operand_list, int operand_count, Assembler_state* a);

static const FunctionTableElement directiveTable[] = {
  {".global", DIRECTIVE_GLOBAL},
  {".extern", DIRECTIVE_EXTERN},
  {".section", DIRECTIVE_SECTION},
  {".word", DIRECTIVE_WORD},
  {".skip", DIRECTIVE_SKIP},
  {".ascii", DIRECTIVE_ASCII},
  {".equ", DIRECTIVE_EQU},
  {".end", DIRECTIVE_END}
};
static const FunctionTableElement opcodeTable[] = {
  {"halt",  OPCODE_HALT},
  {"int",   OPCODE_INT},
  {"iret",  OPCODE_IRET},
  {"call",  OPCODE_CALL},
  {"ret",   OPCODE_RET},
  {"jmp",   OPCODE_JMP},
  {"beq",   OPCODE_BEQ},
  {"bne",   OPCODE_BNE},
  {"bgt",   OPCODE_BGT},
  {"push",  OPCODE_PUSH},
  {"pop",   OPCODE_POP},
  {"xchg",  OPCODE_XCHG},
  {"add",   OPCODE_ADD},
  {"sub",   OPCODE_SUB},
  {"mul",   OPCODE_MUL},
  {"div",   OPCODE_DIV},
  {"not",   OPCODE_NOT},
  {"and",   OPCODE_AND},
  {"or",    OPCODE_OR},
  {"xor",   OPCODE_XOR},
  {"shl",   OPCODE_SHL},
  {"shr",   OPCODE_SHR},
  {"ld",    OPCODE_LD},
  {"st",    OPCODE_ST},
  {"csrrd", OPCODE_CSRRD},
  {"csrwr", OPCODE_CSRWR}
};
#endif