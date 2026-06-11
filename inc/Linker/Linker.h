#ifndef LINKER_H
#define LINKER_H

#include "Command_line_args.h"

typedef enum{
  PASS,
  SIGNATURE_FAKE
}LinkERROR;

void Linker(Linker_init* lI);

#endif