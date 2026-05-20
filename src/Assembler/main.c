#include "../../misc/parser.tab.h"
#include "../../inc/Assembler/CLI_Parser.h"
#include "../../misc/instructions.h"

extern FILE* yyin;

extern Instruction *instructions;
extern int instruction_count;

int main(int argc, const char** argv){
  Files* files = parse(argc, argv);
  
  if(files->type == HELP){
    printf("HELP");
    return 0;
  }
  if(files->type == ERROR){
    printf("Error unknown option");
    return 0;
  }
  
  yyin = fopen(files->input, "r");

  if(!yyin){
    printf("Error file not found: %s\n", files->input);
    return 0;
  }

  yyparse();

  printf("TOTAL: %d\n", instruction_count);
  for (int i = 0; i < instruction_count; i++) {
    Instruction *ins = &instructions[i];

    printf("Instruction %d:\n", i);

    printf("  label = %s\n",
           ins->label ? ins->label : "NULL");

    printf("  mnemonic:\n");
    printf("    type = %d\n", ins->mnemonic.type);
    printf("    id   = %s\n", ins->mnemonic.id);

    printf("  operand_count = %d\n", ins->operand_count);

    if (ins->operand_list && ins->operand_count > 0) {
        for (int j = 0; j < ins->operand_count; j++) {
            Operand *op = &ins->operand_list[j];

            printf("    operand %d:\n", j);
            printf("      type = %d\n", op->type);
            printf("      deref = %d\n", op->dereferance);

            if (op->type == IMM) {
                printf("      immediate = %d\n", op->immediate);
            } 
            else if (op->type == SYM) {
                printf("      symbol = %s\n", op->symbol);
            } 
            else if (op->type == REG) {
                printf("      reg = %s\n", op->reg);
            }
            else if (op->type == STR) {
                printf("   string = %s\n", op->string);
            }
        }
    } else {
        printf("  operands = NULL\n");
    }

    printf("\n");
  }

  
  free(files);
  return 0;
}