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

typedef struct{
  ItemType type;
  union{
    long number;
    char* symbol;
    char* reg;
    long op;
  };
} StackItem;

#endif