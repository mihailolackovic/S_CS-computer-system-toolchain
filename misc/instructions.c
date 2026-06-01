#include "instructions.h"

Expression* create_expression(StackItem* stack){
  printf("STACK START:  ");
  for(int i = 1;i<stack[0].op;i++){
    switch (stack[i].type)
    {
    case ITEM_NUMBER:
      printf("%ld,",stack[i].number);
      break;
    case ITEM_OPERATION:
      printf("%ld,",stack[i].op);
      break;
    case ITEM_REGISTER:
      printf("%s,",stack[i].reg);
      break;
    case ITEM_SYMBOL:
      printf("%s,",stack[i].symbol);
      break;
    default:
      break;
    }
  }
  printf("STACK END\n");

  Operand operand1;
  operand1.type = OPERAND_TYPE_REGISTER;
  operand1.reg = "r1";
  operand1.dereferance_flag = 1;

  Operand operand2;
  operand2.type = OPERAND_TYPE_SYMBOL;
  operand2.symbol = "symbol";
  operand2.dereferance_flag = 0;

  long operand3 = 0;

  Operation operation1 = OPERATION_TYPE_ADD;
  Operation operation2 = OPERATION_TYPE_MUL;

  Expression* expression = malloc(sizeof(Expression));
  expression->operand1 = operand1;
  expression->operand2 = operand2;
  expression->operand3 = operand3;
  expression->operation1 = operation1;
  expression->operation2 = operation2;
  return expression;
};