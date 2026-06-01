#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "keywords.h"

typedef struct Expression Expression;

typedef enum{
  OPERAND_TYPE_EXPRESSION,
  OPERAND_TYPE_NUMBER,
  OPERAND_TYPE_SYMBOL,
  OPERAND_TYPE_REGISTER,
  OPERAND_TYPE_STRING
} OperandType;

typedef struct{
  OperandType type;
  union{
    struct Expression* expression;
    long number;
    char* symbol;
    char* reg;
    char* string;
  };
  int dereferance_flag;
} Operand;

typedef enum{
  OPERATION_TYPE_NONE,
  OPERATION_TYPE_ADD,
  OPERATION_TYPE_SUB,
  OPERATION_TYPE_MUL,
  OPERATION_TYPE_DIV,
  OPERATION_TYPE_OPEN_BRACKET,
  OPERATION_TYPE_CLOSE_BRACKET
} Operation;

typedef struct Expression{
  Operand operand1;
  Operand operand2;
  long operand3;
  long operand4;
  long operand5;
  long operand6;
  Operation operation1;
  Operation operation2;
  Operation operation3;
  Operation operation4;
  Operation operation5;
} Expression;   // ((operand1 operation1 operand3) operation2 operand4) operation3
                // ((operand2 operation4 operand5) operation5 operand6)
typedef struct{
  char* label;
  char* function;
  Operand* operand_list;
  int operand_count;
} Instruction;

Expression* create_expression(StackItem* stack);

#endif