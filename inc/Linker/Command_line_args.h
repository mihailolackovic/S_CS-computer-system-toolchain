#ifndef COMMAND_LINE_ARGS_H
#define COMMAND_LINE_ARGS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum __attribute__((__packed__)){
  LINKER_PASS,
  SIGNATURE_FAKE,
  FILE_BROKEN,
  MALLOC_FAILED,
  LINK_ERROR_UNDEFINED_SYMBOL,
  LINK_ERROR_MULTIPLE_DEFINITION,
  LINK_ERROR_RELOCATION_OVERFLOW,
  LINKER_ERROR_TYPES_MISSMATCH
}LinkERROR;

typedef struct{
  char* section;
  unsigned long address;
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
  char** inputName;
  unsigned short input_count;
  unsigned short place_count;
  Linker_arg arg;
  FILE* output;
  char* outputT;
  FILE* outputT1F;
  char* outputT1;
} Linker_init;

void parse(int argc, const char** argv, Linker_init* lI);

#endif