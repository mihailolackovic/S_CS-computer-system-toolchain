#include "../../misc/parser.tab.h"
#include "../../inc/Assembler/CLI_Parser.h"
#include "../../inc/Assembler/Assemble.h"
#define DEBUG_PARSER

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
  #ifdef DEBUG_PARSER
  for(int i = 0;i<instruction_count;i++){
    printf("%s  ", (instructions[i].label)?instructions[i].label:"NO_LABEL");
    printf("%s  ", instructions[i].function);
    for(int j = 0; j < instructions[i].operand_count; j++){
      switch (instructions[i].operand_list[j].type)
      {
      case OPERAND_TYPE_EXPRESSION:
        if(instructions[i].operand_list[j].dereferance_flag)  printf("[");
        if(instructions[i].operand_list[j].expression->operation1 != OPERATION_TYPE_NONE) printf("(");
        printf("%s",instructions[i].operand_list[j].expression->operand1.reg);
        switch (instructions[i].operand_list[j].expression->operation1)
        {
        case OPERATION_TYPE_NONE:
          break;
        case OPERATION_TYPE_ADD:
          printf("+%s",instructions[i].operand_list[j].expression->operand2.reg);
          break;
        case OPERATION_TYPE_SUB:
          printf("-%s",instructions[i].operand_list[j].expression->operand2.reg);
          break;
        case OPERATION_TYPE_MUL:
          printf("*%s",instructions[i].operand_list[j].expression->operand2.reg);
          break;
        case OPERATION_TYPE_DIV:
          printf("/%s",instructions[i].operand_list[j].expression->operand2.reg);
          break;
        default:
          break;
        }
        if(instructions[i].operand_list[j].expression->operation1 != OPERATION_TYPE_NONE) printf(")");
        if(instructions[i].operand_list[j].expression->operation2 != OPERATION_TYPE_NONE){
          switch (instructions[i].operand_list[j].expression->operation2)
          {
          case OPERATION_TYPE_NONE:
            break;
          case OPERATION_TYPE_ADD:
            printf("+%ld",instructions[i].operand_list[j].expression->operand3);
            break;
          case OPERATION_TYPE_SUB:
            printf("-%ld",instructions[i].operand_list[j].expression->operand3);
            break;
          case OPERATION_TYPE_MUL:
            printf("*%ld",instructions[i].operand_list[j].expression->operand3);
            break;
          case OPERATION_TYPE_DIV:
            printf("/%ld",instructions[i].operand_list[j].expression->operand3);
            break;
          default:
            break;
          }
        }
        if(instructions[i].operand_list[j].dereferance_flag)  printf("]");
        printf("  ");
        break;
      case OPERAND_TYPE_STRING:
        printf("%s  ", instructions[i].operand_list[j].string);
        break;
      case OPERAND_TYPE_SYMBOL:
        printf("%s  ", instructions[i].operand_list[j].symbol);
        break;
      case OPERAND_TYPE_NUMBER:
        printf("%d  ", instructions[i].operand_list[j].number);
        break;
      case OPERAND_TYPE_REGISTER:
        printf("%s  ", instructions[i].operand_list[j].reg);
        break;
      
      default:
        break;
      }
    }
    printf("\n");
  }
  #endif //DEBUG_PARSER

  Assemble(instructions, instruction_count);

  free(files);
  return 0;
}