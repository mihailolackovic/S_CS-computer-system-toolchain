#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "keywords.h"

typedef Operand;

typedef enum{
  OPERATION_TYPE_NONE,
  OPERATION_TYPE_ADD,
  OPERATION_TYPE_SUB,
  OPERATION_TYPE_MUL,
  OPERATION_TYPE_DIV
} Operation;

typedef struct{
  Operand operand1;
  Operand operand2;
  int operand3;
  Operation operation1;
  Operation operation2;
} Expression;   // (operand1 operation1 operand2) operation2 operand3

typedef enum{
  OPERAND_TYPE_EXPRESSION,
  OPERAND_TYPE_NUMBER,
  OPERAND_TYPE_SYMBOL,
  OPERAND_TYPE_REGISTER,
  OPERAND_TYPE_STRING
} OperandType;

struct Operand{
  OperandType type;
  union{
    Expression* expression;
    long number;
    char* symbol;
    char* reg;
    char* string;
  };
  int dereferance_flag;
};

typedef struct{
  char* label;
  char* func;
  Operand* operand_list;
  int operand_count;
} Instruction;

#endif