#include "instructions.h"

void debug1(StackItem* stack){
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
};

void push(Stack* stack, StackItem item){
	if(stack->top >= stack->size - 1)
		printf("ERROR STACK OVERFLOW\n");
	stack->stack[++stack->top] = item;
};

StackItem pop(Stack* stack){
	if(stack->top < 0)
		printf("ERROR STACK UNDERFLOW\n");
	return stack->stack[stack->top--];
};

int is_empty(Stack* stack){
	return (stack->top == -1);
};
StackItem peek(Stack* stack){
	if(stack->top < 0)
		printf("ERROR PEEK AT EMPTY STACK\n");
	return stack->stack[stack->top];
};

int get_IPR(Operation op) {
  switch (op) {
    case OPERATION_TYPE_ADD:
    case OPERATION_TYPE_SUB:           return 2;
    case OPERATION_TYPE_MUL:
    case OPERATION_TYPE_DIV:           return 3;
    case OPERATION_TYPE_OPEN_BRACKET:  return 6;
    case OPERATION_TYPE_CLOSE_BRACKET: return 1;
    default:                           return 0;
  }
}

int get_SPR(Operation op) {
  switch (op) {
    case OPERATION_TYPE_ADD:
    case OPERATION_TYPE_SUB:           return 2;
    case OPERATION_TYPE_MUL:
    case OPERATION_TYPE_DIV:           return 3;
    case OPERATION_TYPE_OPEN_BRACKET:  return 0;
    case OPERATION_TYPE_CLOSE_BRACKET: return -1;
    default:                           return 0;
  }
}

int get_R(Operation op) {
  switch (op) {
    case OPERATION_TYPE_ADD:
    case OPERATION_TYPE_SUB:
    case OPERATION_TYPE_MUL:
    case OPERATION_TYPE_DIV:           return -1;
    case OPERATION_TYPE_OPEN_BRACKET:
    case OPERATION_TYPE_CLOSE_BRACKET: return 0;
    default:                           return 0;
  }
}

Expression create_expression(StackItem* infix) {
	int infix_length = infix[0].op - 1;
	StackItem postfix[infix_length * 2]; // Give it plenty of room for structural brackets
	infix = infix + 1;

	int rank = 0;
	int out_idx = 0;

	StackItem stack_mem[128];
	Stack stack = (Stack){stack_mem, -1, 128};

	for (int i = 0; i < infix_length; i++) {
		StackItem next = infix[i];

		if (next.type == ITEM_NUMBER || next.type == ITEM_SYMBOL || next.type == ITEM_REGISTER) {
			postfix[out_idx++] = next;
			rank = rank + 1;
		} 
		else if (next.type == ITEM_OPERATION) {
			while (!is_empty(&stack) && (get_IPR(next.op) <= get_SPR(peek(&stack).op))) {
				StackItem x = pop(&stack);
				postfix[out_idx++] = x;
				rank = rank + get_R(x.op);
				
				if (rank < 1)
					printf("ERROR INVALID EXPRESSION (Rank < 1)\n");
			}

			if (next.op != OPERATION_TYPE_CLOSE_BRACKET) {
				push(&stack, next);
			} else {
				if (is_empty(&stack) || peek(&stack).op != OPERATION_TYPE_OPEN_BRACKET)
					printf("ERROR INVALID EXPRESSION (Mismatched parens)\n");
				pop(&stack);
			}
		}
	}

	while (!is_empty(&stack)) {
		StackItem x = pop(&stack);
		postfix[out_idx++] = x;
		rank = rank + get_R(x.op);
	}
	
	if (rank != 1)
		printf("ERROR INVALID EXPRESSION (Final rank was %d)\n", rank);

	int count = 0;
	for (int i = 0; i < out_idx; i++) {
		if (postfix[i].type != ITEM_OPERATION || 
		 (postfix[i].op != OPERATION_TYPE_OPEN_BRACKET && postfix[i].op != OPERATION_TYPE_CLOSE_BRACKET)) {
			count++;
		}
	}

	StackItem* result = malloc(sizeof(StackItem) * count);
	int r_idx = 0;
	for (int i = 0; i < out_idx; i++) {
		if (postfix[i].type != ITEM_OPERATION || 
		 (postfix[i].op != OPERATION_TYPE_OPEN_BRACKET && postfix[i].op != OPERATION_TYPE_CLOSE_BRACKET)) {
			result[r_idx++] = postfix[i];
		}
	}

	return (Expression){result, count};
}