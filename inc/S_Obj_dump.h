#ifndef S_OBJ_DUMP_H
#define S_OBJ_DUMP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "./common/ObjectFile_elements.h"
#include "./common/emit.h"

typedef struct {
  int address;
  int size;
  uint8_t type;
  uint8_t binding;
  char* name;
} HexSym;

int readO(FILE* input, FILE* output);
int readHEX(FILE* input, FILE* output);

char* instructionPrinter(unsigned int instruction);

char* gprtos(uint8_t reg);
char* csrtos(uint8_t reg);

#endif