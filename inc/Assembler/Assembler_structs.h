#ifndef ASSEMBLER_STRUCTS_H
#define ASSEMBLER_STRUCTS_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef enum{
  PASS,
  INVALID_OPERAND,
  DOUBLE_SYMBOL
} AsmERROR;

////////////////////////////////////////////////////////////
//  SYMBOLS  ///////////////////////////////////////////////
////////////////////////////////////////////////////////////


typedef struct{
  char* name;
  int base;
  int lenght;
} Section;

AsmERROR addSection(Section section, Section* sectionTable);
AsmERROR findSection(Section section, Section* sectionTable);

typedef enum{
  DATA,
  FUNC,
  SECT,
  SRC
} SymbolType;
typedef enum{
  GLOBAL,
  LOCAL
} SymbolBinding;

typedef struct{
  char* name;
  int value;
  int size;
  SymbolType type;
  SymbolBinding binding;
  Section* section;
} Symbol;

AsmERROR addSymbol(Symbol symbol, Symbol* symbolTable);
AsmERROR findSymbol(Symbol symbol, Symbol* symbolTable);

typedef struct{
  FILE* output;
  int assembler_flag;
  char* current_section;
  int LC;
  Symbol* symbolTable;
  Section* sectionTable;
} Assembler_state;

////////////////////////////////////////////////////////////
//  FUNCS  /////////////////////////////////////////////////
////////////////////////////////////////////////////////////
typedef enum{
  DIRECTIVE_GLOBAL,
  DIRECTIVE_EXTERN,
  DIRECTIVE_SECTION,
  DIRECTIVE_WORD,
  DIRECTIVE_SKIP,
  DIRECTIVE_ASCII,
  DIRECTIVE_EQU,
  DIRECTIVE_END,

  OPCODE_HALT,
  OPCODE_INT,
  OPCODE_IRET,
  OPCODE_CALL,
  OPCODE_RET,
  OPCODE_JMP,
  OPCODE_BEQ,
  OPCODE_BNE,
  OPCODE_BGT,
  OPCODE_PUSH,
  OPCODE_POP,
  OPCODE_XCHG,
  OPCODE_ADD,
  OPCODE_SUB,
  OPCODE_MUL,
  OPCODE_DIV,
  OPCODE_NOT,
  OPCODE_AND,
  OPCODE_OR,
  OPCODE_XOR,
  OPCODE_SHL,
  OPCODE_SHR,
  OPCODE_LD,
  OPCODE_ST,
  OPCODE_CSRRD,
  OPCODE_CSRWR,

  FUNCTION_INVALID
} Functions;

typedef struct{
  char* name;
  Functions dir;
} FunctionTableElement;

#endif