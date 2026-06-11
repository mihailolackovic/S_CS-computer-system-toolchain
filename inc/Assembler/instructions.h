#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdio.h>
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

typedef struct Expression {
  StackItem* arr;
  int arr_size;
} Expression;

typedef struct{
  char* label;
  char* function;
  Operand* operand_list;
  int operand_count;
} Instruction;

Expression create_expression(StackItem* stack);

StackItem pop(Stack* stack);
void push(Stack* stack, StackItem item);
int is_empty(Stack* stack);
StackItem peek(Stack* stack);

#endif