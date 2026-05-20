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

#endif