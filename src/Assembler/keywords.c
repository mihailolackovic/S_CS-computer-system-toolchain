#include "keywords.h"
#include <string.h>

static char* opcode_list[] = {
  "halt",
  "int",
  "iret",
  "call",
  "ret",
  "jmp",
  "beq",
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
  ".weak",
  ".skip",
  ".ascii",
  ".equ",
  ".end",
  ".type",
  NULL // footer
};

static char* registers_list[] = {
  "r1",
  "r2",
  "r3",
  "r4",
  "r5",
  "r6",
  "r7",
  "r8",
  "r9",
  "r10",
  "r11",
  "r12",
  "r13",
  "r13",
  "r15",
  "sp",
  "pc",
  NULL  // footer
};

Token lookup_identifier(const char *s){
  for(int i = 0; opcode_list[i]; i++){
    if(strcmp(opcode_list[i], s) == 0)
      return TYPE_OPCODE;
  }
  for(int i = 0; registers_list[i]; i++){
    if(strcmp(registers_list[i], s) == 0)
      return TYPE_REG;
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