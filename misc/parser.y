%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "instructions.h"

int yylex(void);
void yyerror(const char *s);

Instruction *instructions = NULL;
int instruction_count = 0;
int instruction_capacity = 0;

char *pending_label = NULL;

static void add_instruction(Instruction ins) {
    if (instruction_count >= instruction_capacity) {
        instruction_capacity = instruction_capacity ? instruction_capacity * 2 : 64;
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

%token <str> REGISTER SYMBOL OP_CODE

%start program

%%
program
    : program line
    | /* empty */
    ;

line
    : END_LINE
    | directive END_LINE
    | instruction END_LINE
    | label_line END_LINE
    ;

directive
    : DIRECTIVE
      {
          Instruction ins;

          ins.label = NULL;
          ins.mnemonic.type = DIRECTIVE_TYPE;
          ins.mnemonic.id = $1 + 1;
          ins.operand_list = NULL;
          ins.operand_count = 0;

          add_instruction(ins);
      }
    ;

label_line
    : LABEL
      {
          printf("LABEL: %s\n", $1);
      }
    ;

instruction
    : OP_CODE
      {
          printf("OPCODE: %s\n", $1);
      }
    ;
%%

void yyerror(const char *s) {
    fprintf(stderr, "Parse error: %s\n", s);
}