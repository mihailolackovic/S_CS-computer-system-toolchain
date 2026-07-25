#include "../../inc/Linker/Command_line_args.h"

void parse(int argc, const char** argv, Linker_init* lI){
  *lI = (Linker_init){NULL, NULL, NULL, 0, 0, LINKER_ARG_NONE, NULL, NULL, 0, 10};

  for(int i = 1; i < argc; i++){
    if(strcmp(argv[i], "-o") == 0){
      i++;
      lI->outputT = strdup(argv[i]);
      lI->outputT1 = strdup(lI->outputT);
      strcat(lI->outputT1, ".1.txt");
      lI->output = fopen(argv[i], "wb+");
      if(lI->output == NULL){
        lI->arg = LINKER_ARG_FILE_OPENING_ERROR;
        return;
      }
    }
    else if (strcmp(argv[i], "-hex") == 0){
      if(lI->arg == LINKER_ARG_RELOCATABLE){
        lI->arg = LINKER_ARG_H_AND_R;
        return;
      }
      lI->arg = LINKER_ARG_HEX;
    }
    else if (strcmp(argv[i], "-relocatable") == 0){
      if(lI->arg == LINKER_ARG_HEX){
        lI->arg = LINKER_ARG_H_AND_R;
        return;
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
      if(argv[i] == '-'){
        lI->arg = LINKER_ARG_UNKOWN_OPT;
        return;
      }
      lI->input_count++;
      lI->input = realloc(lI->input, lI->input_count * sizeof(FILE*));
      lI->inputName = realloc(lI->inputName, lI->input_count * sizeof(char*));
      lI->input[lI->input_count - 1] = fopen(argv[i], "rb+");
      if(lI->input == NULL){
        lI->arg = LINKER_ARG_FILE_OPENING_ERROR;
        return;
      }
      lI->inputName[lI->input_count - 1] = strdup(argv[i]);
    }
  }

  if(lI->output == NULL){
    lI->outputT = strdup(lI->inputName[0]);
    lI->outputT1 = strdup(lI->outputT);
    strcat(lI->outputT1, "1.txt");
    strcat(lI->outputT, ".hex");
    lI->output = fopen(lI->outputT, "wb+");
  }

  if(lI->arg == LINKER_ARG_HEX){
    lI->outputT1F = fopen(lI->outputT1, "w");
  }
  else
    lI->outputT1F = NULL;

  return;
};