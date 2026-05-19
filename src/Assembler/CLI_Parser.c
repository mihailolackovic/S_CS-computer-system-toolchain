#include "../../inc/Assembler/CLI_Parser.h"

Files* parse(int argc, const char** argv){
  Files* files = malloc(sizeof(Files));
  files->input = NULL;
  files->output = NULL;
  files->type = ERROR;

  for(int i = 1; i < argc; i++){
    if(strcmp(argv[i], "-o") == 0){
      if(i + 1 < argc)
        files->output = argv[++i];
    }
    else if(strcmp(argv[i], "-help") == 0){
      files->type = HELP;
      return files;
    }
    else if(argv[i][0] == '-'){
      files->type == ERROR;
      return files;
    }
    else
      files->input = argv[i];
  }

  if (files->input != NULL)
    if(files->output != NULL)
      files->type = OUTFILE;
    else{
      files->output = malloc(strlen(files->input) + 3);
      sprintf(files->output, "%s.o", files->input);
      files->type = DEFAULT;
    }
  else
    files->type = ERROR;

  return files;
};