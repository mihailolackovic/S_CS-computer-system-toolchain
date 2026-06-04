#ifndef EMIT_H
#define EMIT_H

#include <stdio.h>
#include <stdint.h>

void emit1(FILE *out, uint8_t x);
void emit2(FILE *out, uint16_t x);
void emit3(FILE *out, uint32_t x);
void emit4(FILE *out, uint32_t x);
void emit8(FILE *out, uint64_t x);

#endif