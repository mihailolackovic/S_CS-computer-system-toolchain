#include "../../misc/parser.tab.h"
#include "../../inc/Assembler/CLI_Parser.h"
#include "../../misc/instructions.h"

extern FILE* yyin;

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
  
  free(files);
  return 0;
}