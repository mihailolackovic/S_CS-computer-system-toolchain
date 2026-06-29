#ifndef EMULATOR_STRUCTS_H
#define EMULATOR_STRUCTS_H

typedef enum{
  NO_MASK = 0,
  Timer = 1,
  Terminal = 2,
  Global = 4
} Status;

typedef enum{
  NO_CAUSE = 0,
  FAULT = 1,
  ISR_TIMER = 2,
  ISR_TERMINAL = 3,
  SYSCALL = 4
} Cause;

typedef enum{
  term_out = 0xFFFFFF00,
  term_in = 0xFFFFFF04,
  timer_cfg = 0xFFFFFF10
} MMIO;

typedef enum{
  sp = 14,
  pc = 15,

  status = 0,
  handler = 1,
  cause = 2
} Reg;

#endif