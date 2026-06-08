#ifndef ASSEMBLER_STRUCTS_H
#define ASSEMBLER_STRUCTS_H

#include "../../misc/instructions.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef enum{
  PASS,
  TABLE_FULL,
  INVALID_OPERAND,
  DOUBLE_SYMBOL,
  DOUBLE_SECTION,
  EXTERN_SYMBOL_REDEFINITION,
  SYMBOL_NOT_IN_SECTION,
  NUMBER_OVERFLOW
} AsmERROR;

#define SYMBOL_TABLE_LENGHT 128
#define SECTION_TABLE_LENGHT 32
#define RELOC_TABLE_LENGHT 128

typedef struct Fixup{
  int offset;
  struct Fixup* next;
} Fixup;

////////////////////////////////////////////////////////////
//  RELOC  /////////////////////////////////////////////////
////////////////////////////////////////////////////////////

typedef enum{
  UNINITIALISED,
  R_ABS_12,
  R_PC_12,
  R_ABS_32,
  R_PC_32
} RelocType;

typedef struct{
  int section;
  int symbol;
  RelocType type;
  int offset;
  int addend;
  int multiplier;
} Relocation;

AsmERROR addRelocation(Relocation relocation, Relocation* relocationTable);

////////////////////////////////////////////////////////////
//  SECTION  ///////////////////////////////////////////////
////////////////////////////////////////////////////////////

typedef struct{
  char* name;
  int base;
  int lenght;
} Section;

int addSection(Section section, Section* sectionTable);
int findSection(char* section, Section* sectionTable);

////////////////////////////////////////////////////////////
//  SYMBOLS  ///////////////////////////////////////////////
////////////////////////////////////////////////////////////

typedef enum{
  NOTYPE,
  DATA,
  FUNC,
  SECT,
  SRC
} SymbolType;
typedef enum{
  LOCAL,
  GLOBAL,
  EXTERN
} SymbolBinding;

typedef struct{
  char* name;
  int value;
  int size;
  SymbolType type;
  SymbolBinding binding;
  int section;
  Fixup* fixup;
} Symbol;

AsmERROR addSymbol(Symbol symbol, Symbol* symbolTable);
int findSymbol(char* symbol, Symbol* symbolTable);

////////////////////////////////////////////////////////////

typedef struct{
  FILE* output;
  int assembler_flag;
  int LC;
  char* current_section;
  Symbol* symbolTable;
  Section* sectionTable;
  Relocation* relocTable;
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

////////////////////////////////////////////////////////////

int registerIndex(char* reg);

typedef struct ForwardDeclaration{
  struct ForwardDeclaration* next;
  Expression* expression;
  char* symbol;
} ForwardDeclaration;

#endif