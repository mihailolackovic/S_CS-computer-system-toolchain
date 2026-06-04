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


  Element element1;
  element1.type = OPERAND_TYPE_SYMBOL;
  element1.symbol = "symbol";
  element1.coeficient = 2;
  element1.operation = OPERATION_TYPE_ADD;
  
  Element element2;
  element2.type = OPERAND_TYPE_NUMBER;
  element2.symbol = NULL;
  element2.coeficient = 89;
  element2.operation = OPERATION_TYPE_NONE;

  Expression* expression = malloc(sizeof(Expression));
  expression->arr = malloc(2*sizeof(Element));
  expression->arr[0] = element1;
  expression->arr[1] = element2;
  expression->arr_size = 2;
  return expression;
};