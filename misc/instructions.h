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

typedef enum{
  OPERATION_TYPE_NONE,
  OPERATION_TYPE_ADD,
  OPERATION_TYPE_SUB,
  OPERATION_TYPE_MUL,
  OPERATION_TYPE_DIV,
  OPERATION_TYPE_OPEN_BRACKET,
  OPERATION_TYPE_CLOSE_BRACKET
} Operation;

typedef struct  {
  OperandType type;
  char* symbol; //  value(symbol) = (symbol == NULL)? 1:value(symbol) 
  long coeficient;
  Operation operation;
} Element;  //  operand (coefficient * symbol) Element


typedef struct Expression {
  Element* arr;
  int arr_size;
} Expression;

typedef struct{
  char* label;
  char* function;
  Operand* operand_list;
  int operand_count;
} Instruction;

Expression* create_expression(StackItem* stack);

#endif