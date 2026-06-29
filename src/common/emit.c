#include "../../inc/common/emit.h"

void emit1(FILE *out, uint8_t x) {
  fwrite(&x, 1, 1, out);
}

void emit2(FILE *out, uint16_t x) {
  emit1(out, x & 0xff);
  emit1(out, (x >> 8) & 0xff);
}

void emit3(FILE *out, uint32_t x) {
  emit1(out, x & 0xff);
  emit1(out, (x >> 8) & 0xff);
  emit1(out, (x >> 16) & 0xff);
}

void emit4(FILE *out, uint32_t x) {
  emit1(out, x & 0xff);
  emit1(out, (x >> 8) & 0xff);
  emit1(out, (x >> 16) & 0xff);
  emit1(out, (x >> 24) & 0xff);
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
};

void instructionEmit(FILE* out, Ins_emit ins){
  uint32_t oc = (uint32_t)(ins.OC & 0xF)  << 28;
  uint32_t m  = (uint32_t)(ins.M  & 0xF)  << 24;
  uint32_t a  = (uint32_t)(ins.A  & 0xF)  << 20;
  uint32_t b  = (uint32_t)(ins.B  & 0xF)  << 16;
  uint32_t c  = (uint32_t)(ins.C  & 0xF)  << 12;
  uint32_t d  = (uint32_t)(ins.D) & 0xFFF;
  
  emit4(out, oc | m | a | b | c | d);
};