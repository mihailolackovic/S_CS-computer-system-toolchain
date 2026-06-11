#include "../../inc/Linker/Command_line_args.h"

Linker_init* parse(int argc, const char** argv){
  Linker_init* lI = malloc(sizeof(Linker_init));
  *lI = (Linker_init){NULL, NULL, 0, 0, LINKER_ARG_NONE, NULL};

  for(int i = 1; i < argc; i++){
    if(strcmp(argv[i], "-o") == 0){
      i++;
      FILE* output = fopen(argv[i], "wb+");
      if(output == NULL){
        lI->arg = LINKER_ARG_FILE_OPENING_ERROR;
        return lI;
      }
      lI->output = output;
    }
    else if (strcmp(argv[i], "-hex") == 0){
      if(lI->arg == LINKER_ARG_RELOCATABLE){
        lI->arg = LINKER_ARG_H_AND_R;
        return lI;
      }
      lI->arg = LINKER_ARG_HEX;
    }
    else if (strcmp(argv[i], "-relocatable") == 0){
      if(lI->arg == LINKER_ARG_HEX){
        lI->arg = LINKER_ARG_H_AND_R;
        return lI;
      }
      lI->arg = LINKER_ARG_RELOCATABLE;
    }
    else if (strncmp(argv[i], "-place=", 7) == 0){
      char *at = strchr(argv[i], '@');
      int sep = at - (argv[i] + 7);
      char name[256];
      strncpy(name, argv[i]+7, sep);
      name[sep] = '\0';
      long num = strtol(at + 1, NULL, 16);
      lI->place_count++;
      lI->place = realloc(lI->place, lI->place_count*sizeof(Place));
      lI->place[lI->place_count-1] = (Place){strdup(name), num};
    }
    else{
      if(argv[0] == '-'){
        lI->arg = LINKER_ARG_UNKOWN_OPT;
        return lI;
      }
      FILE* input = fopen(argv[i], "rb+");
      if(input == NULL){
        lI->arg = LINKER_ARG_FILE_OPENING_ERROR;
        return lI;
      }
      lI->input_count++;
      lI->input = realloc(lI->input, lI->input_count * sizeof(FILE*));
      lI->input[lI->input_count - 1] = input;
    }
  }
  return lI;
};