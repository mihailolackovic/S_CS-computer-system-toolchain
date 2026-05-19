#ifndef KEYWORDS_H
#define KEYWORDS_H

typedef enum {
  ERRORTYPE = 0,
  TYPE_OPCODE,
  TYPE_DIRECTIVE,
  TYPE_SYMBOL
} Token;

Token lookup_opcode(const char *s);
Token lookup_directive(const char *s);

#endif