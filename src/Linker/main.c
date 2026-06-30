#include "../../inc/Linker/Linker.h"

int main(int argc, const char** argv){
  Linker_init* lI = malloc(sizeof(Linker_init));
  
  parse(argc, argv, lI);

  if(lI->arg == LINKER_ARG_FILE_NOT_FOUND){
    printf("\nLINKER: Error fopen failed :( !\n");
    return 1;
  }
  if(lI->arg == LINKER_ARG_H_AND_R){
    printf("\nLINKER: You cannot be bolth relocatable and executable :( !\n");
    return 2;
  }
  if(lI->arg == LINKER_ARG_NONE){
    printf("\nLINKER: You must specify witch file type you want to output:\n -relocatable for relocatable .o file\n -hex for executab;e .hex file\n");
    return 3;
  }
  
  Linker(lI);

  fclose(lI->output);

  char string[255];
  if(argv[0][0] == ".")
    sprintf( string, "./S_Obj_dump %s", lI->outputT);
  else
    sprintf( string, "S_Obj_dump %s", lI->outputT);

  system(string);

  free(lI->place);
  for(int i  = 0; i < lI->input_count; i++)
    fclose(lI->input[i]);
  free(lI->input);
  free(lI->outputT);
  free(lI);
  return 0;
}