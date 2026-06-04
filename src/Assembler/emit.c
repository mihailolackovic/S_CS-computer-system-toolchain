#include "../../inc/Assembler/emit.h"

void emit1(FILE *out, uint8_t x) {
  fwrite(&x, 1, 1, out);
}

void emit2(FILE *out, uint16_t x) {
  emit1(out, (x >> 8) & 0xff);
  emit1(out, x & 0xff);
}

void emit3(FILE *out, uint32_t x) {
  emit1(out, (x >> 16) & 0xff);
  emit1(out, (x >> 8) & 0xff);
  emit1(out, x & 0xff);
}

void emit4(FILE *out, uint32_t x) {
  emit1(out, (x >> 24) & 0xff);
  emit1(out, (x >> 16) & 0xff);
  emit1(out, (x >> 8) & 0xff);
  emit1(out, x & 0xff);
}

void emit8(FILE *out, uint64_t x) {
  emit1(out, (x >> 56) & 0xff);
  emit1(out, (x >> 48) & 0xff);
  emit1(out, (x >> 40) & 0xff);
  emit1(out, (x >> 32) & 0xff);
  emit1(out, (x >> 24) & 0xff);
  emit1(out, (x >> 16) & 0xff);
  emit1(out, (x >> 8) & 0xff);
  emit1(out, x & 0xff);
}