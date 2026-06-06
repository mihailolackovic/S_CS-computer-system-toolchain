#ifndef KEYWORDS_H
#define KEYWORDS_H

typedef enum {
  ERRORTYPE = 0,
  TYPE_OPCODE,
  TYPE_DIRECTIVE,
  TYPE_SYMBOL,
  TYPE_REG
} Token;

Token lookup_identifier(const char *s);
Token lookup_directive(const char *s);

// Stack for evaluating expressions

typedef enum{
  ITEM_NUMBER,
  ITEM_SYMBOL,
  ITEM_REGISTER,
  ITEM_OPERATION
} ItemType;

typedef enum{
  OPERATION_TYPE_NONE,
  OPERATION_TYPE_ADD,
  OPERATION_TYPE_SUB,
  OPERATION_TYPE_MUL,
  OPERATION_TYPE_DIV,
  OPERATION_TYPE_OPEN_BRACKET,
  OPERATION_TYPE_CLOSE_BRACKET
} Operation;

typedef struct{
  ItemType type;
  union{
    long number;
    char* symbol;
    char* reg;
    Operation op;
  };
} StackItem;

typedef struct{
  StackItem* stack;
  int top;
  int size;
} Stack;

#endif