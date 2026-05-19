#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum{
  ERROR = 0,
  OUTFILE,
  DEFAULT,
  HELP
} Arg;

typedef struct {
  char* input;
  char* output;
  Arg type;
} Files;


Files* parse(int argc, const char** argv);

#endif