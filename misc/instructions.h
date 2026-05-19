#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "keywords.h"

typedef enum{
  IMM = 0,
  SYM,
  REG
} Operand_type;

typedef enum{
  DIRECTIVE_TYPE = 0,
  OP_CODE_TYPE
}Mnemonic_type;

typedef struct{
  Operand_type type;
  union{
    int immediate;
    char* symbol;
    Reg reg;
  };
  int dereferance;
} Operand;

typedef struct{
  Mnemonic_type type;
  char* id;
} Mnemonic;


typedef struct{
  char* label;
  Mnemonic mnemonic;
  Operand* operand_list;
  int operand_count;
} Instruction;


#endif