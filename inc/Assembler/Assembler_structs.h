#ifndef ASSEMBLER_STRUCTS_H
#define ASSEMBLER_STRUCTS_H

#include "../../inc/Assembler/instructions.h"
#include "ObjectFile_elements.h"
#include <stdio.h>
#include <stdlib.h>
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
  NUMBER_OVERFLOW,
  INVALID_EXPRESSION,
  EXPRESSION_NOT_RELOCATABLE,
  CYCLICAL_DEPENDENCY,
  LITERAL_INSERTION
} AsmERROR;

AsmERROR addRelocation(Relocation relocation, Relocation* relocationTable);
int findLastReloc(Relocation* relocationTable);

int addSection(Section section, Section* sectionTable);
int findSection(char* section, Section* sectionTable);

AsmERROR addSymbol(Symbol symbol, Symbol* symbolTable);
int findSymbol(char* symbol, Symbol* symbolTable);

typedef struct ForwardDeclaration{
  union{
    Expression* expression;
    char* symbol;
  };
  int count;
  int* dependency;
  OperandType type;
} ForwardDeclaration;

typedef struct{
  FILE* output;
  int assembler_flag;
  int LC;
  int headerOffset;
  char* current_section;
  Symbol* symbolTable;
  Section* sectionTable;
  Relocation* relocTable;
  int uniqueSym;
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

typedef enum{
  status = 0,
  handler = 1,
  cause = 2,
  sp = 14,
  pc = 15
} Register_Special;

int registerIndex(char* reg);
int evalExpression(Expression* exp, Assembler_state* a);

typedef struct{
  int section_counts[2*SECTION_TABLE_LENGHT];
} RelocStackItem;

int evalRelocability(Expression* exp, Assembler_state* a);

#endif