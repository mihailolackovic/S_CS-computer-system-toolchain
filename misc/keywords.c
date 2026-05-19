#include "keywords.h"
#include <string.h>

static char* opcode_list[] = {
  "halt",
  "int",
  "iret",
  "call",
  "ret",
  "jmp",
  "bne",
  "bgt",
  "push",
  "pop",
  "xchg",
  "add",
  "sub",
  "mul",
  "div",
  "not",
  "and",
  "or",
  "xor",
  "shl",
  "shr",
  "ld",
  "st",
  "csrrd",
  "csrwr",
  NULL  // footer
};

static char* directive_list[] = {
  ".global",
  ".extern",
  ".section",
  ".word",
  ".skip",
  ".ascii",
  ".equ",
  ".end",
  NULL // footer
};

Token lookup_identifier(const char *s){
  if(s[0] == 'r')
    if(atoi(s+1) <= 15)
      return TYPE_REG;

  if(strcmp(s, "sp") == 0)
    return TYPE_REG;
  if(strcmp(s, "pc") == 0)
    return TYPE_REG;

  for(int i = 0; opcode_list[i]; i++){
    if(strcmp(opcode_list[i], s) == 0)
      return TYPE_OPCODE;
  }
  return TYPE_SYMBOL;
};

Token lookup_directive(const char *s){
  for(int i = 0; directive_list[i]; i++){
    if(strcmp(directive_list[i], s) == 0)
      return TYPE_DIRECTIVE;
  }
  return ERRORTYPE;
}