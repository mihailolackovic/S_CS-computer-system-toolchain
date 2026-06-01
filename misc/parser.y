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
  | curr_label curr_mnemonic END_LINE
  | curr_label curr_mnemonic args END_LINE
  ;

curr_label
  :
  | LABEL { pending_label = $1; }
  ;

curr_mnemonic
  : DIRECTIVE
  | OP_CODE
  ;

arg
  : expression
  | STRING
  | IND_REG REGISTER
  | IND_REG SYMBOL
  | IND_VAL SYMBOL
  | IND_VAL NUMBER
  | relative_address
  ;

args
  : arg
  | args SEPERATOR arg
  ;

expression
  : SYMBOL
  | NUMBER
  | IND_REG REGISTER
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
  : IDN_OPEN IND_REG REGISTER IDN_CLOSE
  | IDN_OPEN IND_REG REGISTER ADD NUMBER IDN_CLOSE
  | IDN_OPEN IND_REG REGISTER ADD SYMBOL IDN_CLOSE
  | IDN_OPEN IND_REG REGISTER SUB NUMBER IDN_CLOSE
  | IDN_OPEN IND_REG REGISTER SUB SYMBOL IDN_CLOSE
  ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Parse error: %s\n", s);
}