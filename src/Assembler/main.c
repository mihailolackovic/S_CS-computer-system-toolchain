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

  FILE* output = fopen(files->output, "wb");
  if (!output) {
    perror("fopen");
    return;
  }

  Assemble(instructions, instruction_count, output);

  free(files);
  return 0;
}