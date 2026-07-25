%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../inc/Assembler/instructions.h"

int yylex(void);
void yyerror(const char *s);

//  ISNTRUCTION PARAMS
char* pending_label = NULL;

char* function = NULL;

int operand_count = 0;
Operand list[10];

StackItem stack[128];
int stack_counter = 1;
//  INSTRUCTION

Instruction *instructions = NULL;
int instruction_count = 0;
int instruction_capacity = 64;

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

%token END_LINE SEPERATOR IND_REG IND_VAL IDN_OPEN IDN_CLOSE BLK_OPEN BLK_CLOSE
%token ADD SUB MUL DIV BRAKET_OPEN BRAKET_CLOSE SHL SHR
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
  | curr_label curr_function END_LINE {
      
      Instruction instruction;
      instruction.label = pending_label;
      instruction.function = function;
      instruction.operand_list = NULL;
      instruction.operand_count = 0;
      
      add_instruction(instruction);
      
      pending_label = NULL;
    }
  | curr_label curr_function args END_LINE{
      
      Instruction instruction;
      instruction.label = pending_label;
      instruction.function = function;
      instruction.operand_list = (Operand*)malloc(sizeof(Operand)*operand_count);
      memcpy(instruction.operand_list, list, sizeof(Operand)*operand_count);
      instruction.operand_count = operand_count;
      
      add_instruction(instruction);
      
      
      operand_count = 0;
      pending_label = NULL;
    }
  | curr_label curr_function BLK_OPEN args BLK_CLOSE END_LINE{
      
      Instruction instruction;
      instruction.label = pending_label;
      instruction.function = function;
      instruction.operand_list = (Operand*)malloc(sizeof(Operand)*operand_count);
      memcpy(instruction.operand_list, list, sizeof(Operand)*operand_count);
      instruction.operand_count = operand_count;
      
      instruction.operand_list[0].dereferance_flag = 3;
      add_instruction(instruction);
      
      
      operand_count = 0;
      pending_label = NULL;
    }
  ;

curr_label
  :
  | LABEL { pending_label = $1; }
  ;

curr_function
  : DIRECTIVE { function = $1; }
  | OP_CODE { function = $1; }
  ;

arg
  : expression{
      stack[0] = (StackItem){ITEM_OPERATION, stack_counter};

      Operand operand;
      operand.type = OPERAND_TYPE_EXPRESSION;
      operand.expression = malloc(sizeof(Expression));
      *operand.expression = create_expression(stack);
      operand.dereferance_flag = 0;

      list[operand_count++] = operand;
      stack_counter = 1;
    }
  | STRING  {
      Operand operand;
      operand.type = OPERAND_TYPE_STRING;
      operand.string = $1;
      operand.dereferance_flag = 0;

      list[operand_count++] = operand;
    }
  | SYMBOL  {
      Operand operand;
      operand.type = OPERAND_TYPE_SYMBOL;
      operand.string = $1;
      operand.dereferance_flag = 1;

      list[operand_count++] = operand;
    }
  | NUMBER  {
      Operand operand;
      operand.type = OPERAND_TYPE_NUMBER;
      operand.number = $1;
      operand.dereferance_flag = 1;

      list[operand_count++] = operand;
    }
  | IND_REG REGISTER  {
      Operand operand;
      operand.type = OPERAND_TYPE_REGISTER;
      operand.reg = $2;
      operand.dereferance_flag = 0;

      list[operand_count++] = operand;
    }
  | IND_REG SYMBOL  {
      Operand operand;
      operand.type = OPERAND_TYPE_REGISTER;
      operand.reg = $2;
      operand.dereferance_flag = 0;

      list[operand_count++] = operand;
    }
  | IND_VAL SYMBOL  {
      Operand operand;
      operand.type = OPERAND_TYPE_SYMBOL;
      operand.symbol = $2;
      operand.dereferance_flag = 0;

      list[operand_count++] = operand;
    }
  | IND_VAL NUMBER  {
      Operand operand;
      operand.type = OPERAND_TYPE_NUMBER;
      operand.number = $2;
      operand.dereferance_flag = 0;

      list[operand_count++] = operand;
    }
  | relative_address
  ;

args
  : arg
  | args SEPERATOR arg
  ;

expression
  : SYMBOL {
      stack[stack_counter++] = (StackItem){ITEM_SYMBOL, $1};
    }
  | NUMBER {
      stack[stack_counter++] = (StackItem){ITEM_NUMBER, $1};
    }
  | IND_REG REGISTER {
      stack[stack_counter++] = (StackItem){ITEM_REGISTER, $2};
    }
  | expression operand expression
    | BRAKET_OPEN { stack[stack_counter++] = (StackItem){ITEM_OPERATION, 5};
      }expression BRAKET_CLOSE {
        stack[stack_counter++] = (StackItem){ITEM_OPERATION, 6};
      }
  ;

operand
  : ADD {
      stack[stack_counter++] = (StackItem){ITEM_OPERATION, 1};
    }
  | SUB {
      stack[stack_counter++] = (StackItem){ITEM_OPERATION, 2};
    }
  | MUL {
      stack[stack_counter++] = (StackItem){ITEM_OPERATION, 3};
    }
  | DIV {
      stack[stack_counter++] = (StackItem){ITEM_OPERATION, 4};
    }
  ;

relative_address
  : IDN_OPEN IND_REG REGISTER IDN_CLOSE  {
      Operand operand;
      operand.type = OPERAND_TYPE_REGISTER;
      operand.reg = $3;
      operand.dereferance_flag = 1;

      list[operand_count++] = operand;
    }
  | IND_REG REGISTER SHL NUMBER{
      StackItem* si = malloc(sizeof(StackItem)*3);
      si[0].type = ITEM_REGISTER;
      si[0].reg = $2;

      si[2].type = ITEM_NUMBER;
      si[2].number = $4;

      si[1].type = ITEM_OPERATION;
      si[1].op = 5;

      Operand operand;
      operand.type = OPERAND_TYPE_EXPRESSION;
      operand.expression = malloc(sizeof(Expression));
      *operand.expression = (Expression){si, 3}; 
      operand.dereferance_flag = 4;

      list[operand_count++] = operand;
    }
  | IND_REG REGISTER SHR NUMBER{
      StackItem* si = malloc(sizeof(StackItem)*3);
      si[0].type = ITEM_REGISTER;
      si[0].reg = $2;

      si[2].type = ITEM_NUMBER;
      si[2].number = $4;

      si[1].type = ITEM_OPERATION;
      si[1].op = 6;

      Operand operand;
      operand.type = OPERAND_TYPE_EXPRESSION;
      operand.expression = malloc(sizeof(Expression));
      *operand.expression = (Expression){si, 3}; 
      operand.dereferance_flag = 4;

      list[operand_count++] = operand;
    }
  | IDN_OPEN IND_REG REGISTER ADD IND_REG REGISTER IDN_CLOSE{
      StackItem* si = malloc(sizeof(StackItem)*3);
      si[0].type = ITEM_REGISTER;
      si[0].reg = $3;

      si[1].type = ITEM_REGISTER;
      si[1].reg = $6;

      si[2].type = ITEM_OPERATION;
      si[2].op = OPERATION_TYPE_ADD;

      Operand operand;
      operand.type = OPERAND_TYPE_EXPRESSION;
      operand.expression = malloc(sizeof(Expression));
      *operand.expression = (Expression){si, 3}; 
      operand.dereferance_flag = 1;

      list[operand_count++] = operand;
    }
  | IDN_OPEN IND_REG REGISTER ADD NUMBER IDN_CLOSE{
      StackItem* si = malloc(sizeof(StackItem)*3);
      si[0].type = ITEM_REGISTER;
      si[0].reg = $3;

      si[1].type = ITEM_NUMBER;
      si[1].number = $5;

      si[2].type = ITEM_OPERATION;
      si[2].op = OPERATION_TYPE_ADD;

      Operand operand;
      operand.type = OPERAND_TYPE_EXPRESSION;
      operand.expression = malloc(sizeof(Expression));
      *operand.expression = (Expression){si, 3}; 
      operand.dereferance_flag = 1;

      list[operand_count++] = operand;
    }
  | IDN_OPEN IND_REG REGISTER ADD SYMBOL IDN_CLOSE{
      StackItem* si = malloc(sizeof(StackItem)*3);
      si[0].type = ITEM_REGISTER;
      si[0].reg = $3;

      si[1].type = ITEM_SYMBOL;
      si[1].symbol = $5;

      si[2].type = ITEM_OPERATION;
      si[2].op = OPERATION_TYPE_ADD;

      Operand operand;
      operand.type = OPERAND_TYPE_EXPRESSION;
      operand.expression = malloc(sizeof(Expression));
      *operand.expression = (Expression){si, 3}; 
      operand.dereferance_flag = 1;

      list[operand_count++] = operand;
    }
  | IDN_OPEN IND_REG REGISTER SUB NUMBER IDN_CLOSE{
      StackItem* si = malloc(sizeof(StackItem)*3);
      si[0].type = ITEM_REGISTER;
      si[0].reg = $3;

      si[1].type = ITEM_NUMBER;
      si[1].number = $5;

      si[2].type = ITEM_OPERATION;
      si[2].op = OPERATION_TYPE_SUB;

      Operand operand;
      operand.type = OPERAND_TYPE_EXPRESSION;
      operand.expression = malloc(sizeof(Expression));
      *operand.expression = (Expression){si, 3}; 
      operand.dereferance_flag = 1;

      list[operand_count++] = operand;
    }
  | IDN_OPEN IND_REG REGISTER SUB SYMBOL IDN_CLOSE{
      StackItem* si = malloc(sizeof(StackItem)*3);
      si[0].type = ITEM_REGISTER;
      si[0].reg = $3;

      si[1].type = ITEM_SYMBOL;
      si[1].symbol = $5;

      si[2].type = ITEM_OPERATION;
      si[2].op = OPERATION_TYPE_SUB;

      Operand operand;
      operand.type = OPERAND_TYPE_EXPRESSION;
      operand.expression = malloc(sizeof(Expression));
      *operand.expression = (Expression){si, 3}; 
      operand.dereferance_flag = 1;

      list[operand_count++] = operand;
    }
  ;

%%

void yyerror(const char *s) {
  fprintf(stderr, "Parse error: %s\n", s);
  exit(1);
}