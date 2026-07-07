#ifndef EMIT_H
#define EMIT_H

#include <stdio.h>
#include <stdint.h>

typedef struct{
  uint8_t OC;
  uint8_t M;
  uint8_t A;
  uint8_t B;
  uint8_t C;
  uint32_t D;
} Ins_emit;

void emit1(FILE *out, uint8_t x);
void emit2(FILE *out, uint16_t x);
void emit3(FILE *out, uint32_t x);
void emit4(FILE *out, uint32_t x);
void emit8(FILE *out, uint64_t x);

void fprint1(FILE *out, uint8_t x);
void fprint4(FILE *out, uint32_t x);
void fprint8(FILE *out, uint64_t x);

void instructionEmit(FILE* out, Ins_emit ins);

#endif