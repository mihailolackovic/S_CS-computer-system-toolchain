#include "../../inc/Linker/Command_line_args.h"

int main(int argc, const char** argv){
  
  Linker_init* lI = parse(argc, argv);
  

  free(lI->place);
  for(int i  = 0; i < lI->input_count; i++)
    fclose(lI->input[i]);
  free(lI->input);
  fclose(lI->output);
  free(lI);
  return 0;
}