#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "instructions.h"

typedef enum{
  ADD,
  SUB,
  MUL,
  DIV
} Operation;


typedef struct{
  Operand_type type;
  Operation op;
  int constant;
} Expression_part;

typedef struct{
  Expression_part* part1;
  Operand op;
  Expression_part* part2;
} Expression;

#endif