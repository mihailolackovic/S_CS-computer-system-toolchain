%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "instructions.h"

int yylex(void);
void yyerror(const char *s);

Instruction *instructions = NULL;
int instruction_count = 0;
int instruction_capacity = 64;

char* pending_label = NULL;

char* mnemonic = NULL;
Mnemonic_type mnemonic_type;

int operand_count = 0;
Operand list[10];

static void add_instruction(Instruction ins) {
  if(!instructions) instructions = malloc(64 * sizeof(Instruction));
  if (instruction_count >= instruction_capacity) {
      instruction_capacity *= 2;
      instructions = realloc(instructions, instruction_capacity * sizeof(Instruction));
  }
  instructions[instruction_count++] = ins;
}
%}

%union {
    int number;
    char *str;
}

%token END_LINE SEPERATOR IND_REG IND_VAL IDN_OPEN IDN_CLOSE
%token ADD SUB MUL DIV BRAKET_OPEN BRAKET_CLOSE
%token <str> DIRECTIVE LABEL
%token <number> NUMBER
%token <str> REGISTER SYMBOL OP_CODE STRING

%type <str> curr_label

%start program

%%

program
  :
  | program line
  ;

line
  : END_LINE
  | curr_label END_LINE { pending_label = $1; }
  | curr_label curr_mnemonic END_LINE {
      Instruction ins;
      ins.label = pending_label;
      ins.mnemonic.type = mnemonic_type;
      ins.mnemonic.id = mnemonic;
      if (operand_count > 0) {
        ins.operand_list = malloc(sizeof(Operand) * operand_count);
        memcpy(ins.operand_list, list, sizeof(Operand) * operand_count);
      } else {
        ins.operand_list = NULL;
      }
      ins.operand_count = operand_count;
      add_instruction(ins);
      pending_label = NULL;
      operand_count = 0;
    }
  | curr_label curr_mnemonic args END_LINE {
      Instruction ins;
      ins.label = pending_label;
      ins.mnemonic.type = mnemonic_type;
      ins.mnemonic.id = mnemonic;
      if (operand_count > 0) {
        ins.operand_list = malloc(sizeof(Operand) * operand_count);
        memcpy(ins.operand_list, list, sizeof(Operand) * operand_count);
      } else {
        ins.operand_list = NULL;
      }
      ins.operand_count = operand_count;
      add_instruction(ins);
      pending_label = NULL;
      operand_count = 0;
    }
  ;

curr_label
  :
  | LABEL { pending_label = $1; }
  ;

curr_mnemonic
  : DIRECTIVE {
      mnemonic = $1;
      mnemonic_type = DIRECTIVE_TYPE;
    }
  | OP_CODE {
      mnemonic = $1;
      mnemonic_type = OP_CODE_TYPE;
    }
  ;

arg
  : expression
  | STRING {
      Operand op;
      op.type = REG;
      op.reg = $1;
      op.dereferance = 0;
      list[operand_count++] = op;
    }
  | IND_REG SYMBOL {
      Operand op;
      op.type = REG;
      op.reg = $2;
      op.dereferance = 0;
      list[operand_count++] = op;
    }
  | IND_VAL SYMBOL {
      Operand op;
      op.type = SYM;
      op.symbol = $2;
      op.dereferance = 1;
      list[operand_count++] = op;
    }
  | IND_VAL NUMBER {
      Operand op;
      op.type = IMM;
      op.immediate = $2;
      op.dereferance = 1;
      list[operand_count++] = op;
    }
  | relative_address
  ;

args
  : arg
  | args SEPERATOR arg
  ;

expression
  : SYMBOL {
      Operand op;
      op.type = SYM;
      op.symbol = $1;
      op.dereferance = 0;
      list[operand_count++] = op;
    }
  | NUMBER {
      Operand op;
      op.type = IMM;
      op.immediate = $1;
      op.dereferance = 0;
      list[operand_count++] = op;
    }
  | expression operand expression
  | BRAKET_OPEN expression BRAKET_CLOSE
  ;

operand
  : ADD
  | SUB
  | MUL
  | DIV
  ;

relative_address
  : IDN_OPEN IND_REG SYMBOL IDN_CLOSE {
      Operand op;
      op.type = REG;
      op.reg = $3;
      op.dereferance = 1;
      list[operand_count++] = op;
    }
  | IDN_OPEN IND_REG SYMBOL ADD NUMBER IDN_CLOSE
  | IDN_OPEN IND_REG SYMBOL ADD SYMBOL IDN_CLOSE
  | IDN_OPEN IND_REG SYMBOL SUB NUMBER IDN_CLOSE
  | IDN_OPEN IND_REG SYMBOL SUB SYMBOL IDN_CLOSE
  ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Parse error: %s\n", s);
}