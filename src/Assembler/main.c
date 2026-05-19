#include "../../misc/lexer.h"
#include "../../inc/Assembler/CLI_Parser.h"

extern FILE* yyin;

int main(int argc, const char** argv){
  Files* files = parse(argc, argv);
  
  if(files->type == HELP){
    printf("HELP");
    return 0;
  }
  if(files->type == ERROR){
    printf("Unkown option");
    return 0;
  }

  printf("%s\n", files->input);
  printf("%s\n", files->output);
  
  yyin = fopen(files->input, "r");

  if(!yyin)
    printf("E");

  yylex();

  free(files);
  return 0;
}