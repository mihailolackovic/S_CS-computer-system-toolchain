#include "../../misc/parser.tab.h"
#include "../../inc/Assembler/CLI_Parser.h"
#include "../../inc/Assembler/Assemble.h"

extern FILE* yyin;
extern Instruction *instructions;
extern int instruction_count;

int main(int argc, const char** argv){
  Files* files = parse(argc, argv);

  if(files->type == ERROR){
    printf("\nASSMEBLER: Error unknown option!\n");
    return 1;
  }

  yyin = fopen(files->input, "r");

  if(!yyin){
    printf("\nASSMEBLER: Error file not found: %s\n", files->input);
    return 2;
  }

  yyparse();

  FILE* output = fopen(files->output, "wb+");
  if (!output) {
    perror("ASSMEBLER: fopen error");
    return;
  }
  
  Assemble(instructions, instruction_count, output);

  fclose(output);

  char string[255];
  sprintf( string, "./S_Obj_dump %s", files->output);

  system(string);
  
  fclose(yyin);
  free(files);
  return 0;
}