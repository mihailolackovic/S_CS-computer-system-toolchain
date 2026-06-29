#ifndef EMULATOR_H
#define EMULATOR_H

#include "Emulator_structs.hpp"

#include <iostream>
#include <cstdint>
#include <unordered_map>
#include <cstdio>
#include <cstring>
#include <pthread.h>
#include <termios.h>
#include <ctime>
#include <unistd.h>

class S_CS{
public:

  S_CS(FILE* inputFile);

  void run();

  uint32_t reg(int index);

private:

  //  S_CS  internals

  uint32_t gpr[16] = {0};
  uint32_t csr[3] = {0};

  std::unordered_map<uint32_t,uint8_t> mem;

  bool running = true;

  //  S_CS read/write mem

  uint32_t mem_read32 (uint32_t addr);
  void mem_write32(uint32_t addr, uint32_t val);
  uint8_t mem_read8  (uint32_t addr);
  void mem_write8 (uint32_t addr, uint8_t val);

  void push32(uint32_t val);
  uint32_t pop32();

  //  S_CS fech/execute

  void step();
  void fireInterrupt(Cause cause);

  // S_CS interrup interface

  volatile bool pending_timer = false;
  volatile bool pending_terminal = false;
  volatile uint8_t terminal_char = 0;
  pthread_mutex_t interrupt_mutex = PTHREAD_MUTEX_INITIALIZER;

  // S_CS terminal peripheral

  uint32_t timer_cfg = 0;
  pthread_t timer_thread;
  static void* timer_fn(void* arg);

  // S_CS timer peripheral

  pthread_t terminal_thread;
  static void* terminal_fn(void* arg);
  struct termios old_termios;
  void set_raw_terminal();
  void restore_terminal();

  // DEBUG
#ifdef DEBUG_EMU
  FILE* output = fopen("Output.txt", "w+");
#endif
#ifdef DEBUG_TIMEOUT
  int f = 0;
#endif
};

void Emulator(FILE* input);

#endif