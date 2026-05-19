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

typedef enum{
  r0 = 0,
  r1,
  r2,
  r3,
  r4,
  r5,
  r6,
  r7,
  r8,
  r9,
  r10,
  r11,
  r12,
  r13,
  r14,
  r15,
} Reg;

#endif