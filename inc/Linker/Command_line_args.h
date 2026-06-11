#ifndef COMMAND_LINE_ARGS_H
#define COMMAND_LINE_ARGS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct{
  char* section;
  long address;
} Place;

typedef enum __attribute__((__packed__)){
  LINKER_ARG_NONE,
  LINKER_ARG_HEX,
  LINKER_ARG_RELOCATABLE,
  LINKER_ARG_H_AND_R,
  LINKER_ARG_UNKOWN_OPT,
  LINKER_ARG_FILE_NOT_FOUND,
  LINKER_ARG_FILE_OPENING_ERROR
} Linker_arg;

typedef struct{
  Place* place;
  FILE** input;
  unsigned short input_count;
  unsigned short place_count;
  Linker_arg arg;
  FILE* output;

} Linker_init;

Linker_init* parse(int argc, const char** argv);

#endif