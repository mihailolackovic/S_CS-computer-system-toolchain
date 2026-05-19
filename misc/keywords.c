#include "keywords.h"
#include <string.h>

typedef struct{
  const char* name;
  Token type;
} Entry;

static Entry opcode_list[] = {
  {"halt", TYPE_OPCODE},
  {"int", TYPE_OPCODE},
  {"iret", TYPE_OPCODE},
  {"call", TYPE_OPCODE},
  {"ret", TYPE_OPCODE},
  {"jmp", TYPE_OPCODE},
  {"bne", TYPE_OPCODE},
  {"bgt", TYPE_OPCODE},
  {"push", TYPE_OPCODE},
  {"pop", TYPE_OPCODE},
  {"xchg", TYPE_OPCODE},
  {"add", TYPE_OPCODE},
  {"sub", TYPE_OPCODE},
  {"mul", TYPE_OPCODE},
  {"div", TYPE_OPCODE},
  {"not", TYPE_OPCODE},
  {"and", TYPE_OPCODE},
  {"or", TYPE_OPCODE},
  {"xor", TYPE_OPCODE},
  {"shl", TYPE_OPCODE},
  {"shr", TYPE_OPCODE},
  {"ld", TYPE_OPCODE},
  {"st", TYPE_OPCODE},
  {"csrrd", TYPE_OPCODE},
  {"csrwr", TYPE_OPCODE},
  {NULL, ERRORTYPE} // footer
};

static Entry directive_list[] = {
  {".global", TYPE_DIRECTIVE},
  {".extern", TYPE_DIRECTIVE},
  {".section", TYPE_DIRECTIVE},
  {".word", TYPE_DIRECTIVE},
  {".skip", TYPE_DIRECTIVE},
  {".ascii", TYPE_DIRECTIVE},
  {".equ", TYPE_DIRECTIVE},
  {".end", TYPE_DIRECTIVE},
  {NULL, ERRORTYPE} // footer
};

Token lookup_opcode(const char *s){
  for(int i = 0; opcode_list[i].name; i++){
    if(strcmp(opcode_list[i].name, s) == 0)
      return TYPE_OPCODE;
  }
  return TYPE_SYMBOL;
};

Token lookup_directive(const char *s){
  for(int i = 0; directive_list[i].name; i++){
    if(strcmp(directive_list[i].name, s) == 0)
      return TYPE_DIRECTIVE;
  }
  return ERRORTYPE;
};