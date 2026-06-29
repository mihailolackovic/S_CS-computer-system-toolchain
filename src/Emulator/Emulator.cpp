#include "../../inc/Emulator/Emulator.hpp"
//#define DEBUG_EMU
//#define DEBUG_TIMEOUT
void Emulator(FILE* input){

  S_CS emulator(input);

  emulator.run();

  printf("\n-----------------------------------------------------------------\n");
  printf("Emulated processor executed halt instruction\n");
  printf("Emulated processor state:\n");
  printf(" r0=0x%08X    r1=0x%08X    r2=0x%08X    r3=0x%08X\n",  emulator.reg(0),emulator.reg(1),emulator.reg(2),emulator.reg(3));
  printf(" r4=0x%08X    r5=0x%08X    r6=0x%08X    r7=0x%08X\n",  emulator.reg(4),emulator.reg(5),emulator.reg(6),emulator.reg(7));
  printf(" r8=0x%08X    r9=0x%08X   r10=0x%08X   r11=0x%08X\n", emulator.reg(8),emulator.reg(9),emulator.reg(10),emulator.reg(11));
  printf("r12=0x%08X   r13=0x%08X   r14=0x%08X   r15=0x%08X\n", emulator.reg(12),emulator.reg(13),emulator.reg(14),emulator.reg(15));

  return;
};



S_CS::S_CS(FILE* inputFile) {
  uint32_t segmentTableOffset;
  fseek(inputFile, 0x08, SEEK_SET);
  fread(&segmentTableOffset, 4, 1, inputFile);

  uint32_t segmentCount;
  fseek(inputFile, segmentTableOffset, SEEK_SET);
  fread(&segmentCount, 4, 1, inputFile);

  for(uint32_t i = 0; i < segmentCount; i++){
    uint32_t load_addr, size, file_off;
    fread(&load_addr, 4, 1, inputFile);
    fread(&size,      4, 1, inputFile);
    fread(&file_off,  4, 1, inputFile);
    if(load_addr == 0xFFFFFFFF) continue;
    long oldCur = ftell(inputFile);
    fseek(inputFile, file_off, SEEK_SET);
    for(uint32_t j = 0; j < size; j++){
      uint8_t byte;
      fread(&byte, 1, 1, inputFile);
      mem[load_addr + j] = byte;
    }
    fseek(inputFile, oldCur, SEEK_SET);
  }

  gpr[15] = 0x40000000;
};

void S_CS::run(){
  set_raw_terminal();

  pthread_create(&terminal_thread, NULL, terminal_fn, this);
  pthread_create(&timer_thread, NULL, timer_fn, this);

  while(running){
    step();
#ifdef DEBUG_TIMEOUT
    if(++f > 2000000){
      fprintf(stderr, "STEP LIMIT HIT - halting for inspection\n");
      break;
    }
#endif
  }
  restore_terminal();

  pthread_cancel(terminal_thread);
  pthread_cancel(timer_thread);
  pthread_join(terminal_thread, NULL);
  pthread_join(timer_thread, NULL);
};

uint32_t S_CS::reg(int index){
  return gpr[index];
};

  //  S_CS read/write mem

uint32_t S_CS::mem_read32(uint32_t addr){
  if(addr == term_in){
    pthread_mutex_lock(&interrupt_mutex);
    uint8_t c = terminal_char;
    pthread_mutex_unlock(&interrupt_mutex);
    return c;
  }
  return  ((uint32_t)mem_read8(addr+0) <<  0) |
          ((uint32_t)mem_read8(addr+1) <<  8) |
          ((uint32_t)mem_read8(addr+2) << 16) |
          ((uint32_t)mem_read8(addr+3) << 24) ;
};

void S_CS::mem_write32(uint32_t addr, uint32_t val){
  if(addr == term_out){
    putchar(val & 0xFF);
    fflush(stdout);
    return;
  }
  if(addr == MMIO::timer_cfg){
    timer_cfg = val;
  }

  mem_write8(addr+0, (uint8_t)(val >>  0) & 0xFF);
  mem_write8(addr+1, (uint8_t)(val >>  8) & 0xFF);
  mem_write8(addr+2, (uint8_t)(val >> 16) & 0xFF);
  mem_write8(addr+3, (uint8_t)(val >> 24) & 0xFF);
};

uint8_t S_CS::mem_read8(uint32_t addr){
  auto it = mem.find(addr);
  return (it != mem.end()) ? it->second : 0;
};

void S_CS::mem_write8(uint32_t addr, uint8_t val){
  mem[addr] = val;
};


void S_CS::push32(uint32_t val){
  gpr[sp] -= 4;
  mem_write32(gpr[sp], val);
};

uint32_t S_CS::pop32(){
  uint32_t val = mem_read32(gpr[sp]);
  gpr[sp] += 4;
  return val;
};

  //  S_CS fech/execute

void S_CS::step(){
#ifdef DEBUG_EMU
  uint32_t dbg_instr = mem_read32(gpr[15]);
  fprintf(output, "PC=0x%08X  INSTR=%08X  sp=0x%08X r1=0x%08X r2=0x%08X r3=0x%08X status=0x%08X haandler=0x%08X caause==0x%08X\n",
    gpr[15], dbg_instr, gpr[14], gpr[1], gpr[2], gpr[3], csr[0], csr[1], csr[2]);
#endif

  if(!(csr[status] & Global)){
    pthread_mutex_lock(&interrupt_mutex);
    bool has_timer = pending_timer;
    bool has_terminal = pending_terminal;
    uint8_t tchar = terminal_char;
    pthread_mutex_unlock(&interrupt_mutex);

    if(has_timer && !(csr[status] & Timer)){
      pthread_mutex_lock(&interrupt_mutex);
      pending_timer = false;
      pthread_mutex_unlock(&interrupt_mutex);
      fireInterrupt(ISR_TIMER);
      return;
    }
    if(has_terminal && !(csr[status] & Terminal)){
      pthread_mutex_lock(&interrupt_mutex);
      pending_terminal = false;
      pthread_mutex_unlock(&interrupt_mutex);
      //mem_write32(term_in, tchar);
      fireInterrupt(ISR_TERMINAL);
      return;
    }
  }

  uint32_t instruction = mem_read32(gpr[pc]);
  gpr[pc] += 4;
  gpr[0] = 0;

  uint8_t OC = (instruction >> 28) & 0xF;
  uint8_t M  = (instruction >> 24) & 0xF;
  uint8_t A  = (instruction >> 20) & 0xF;
  uint8_t B  = (instruction >> 16) & 0xF;
  uint8_t C  = (instruction >> 12) & 0xF;
  int32_t D = (instruction >>  0) & 0xFFF;
  if(D & 0x800) D -= 0x1000;

  uint32_t tmp;
  uint32_t target;

  bool taken;
  bool indir;
  int cond;


  switch(OC)
  {
  case 0x0:
    running = false;
    break;

  case 0x1:
    push32(gpr[pc]);
    push32(csr[status]);
    csr[cause] = 4;
    csr[status] |= Global;
    gpr[pc] = csr[handler];
    break;

  case 0x2:
    push32(gpr[pc]);
    if(M == 0x0) gpr[pc] = gpr[A] + gpr[B] + D;
    else if(M == 0x1) gpr[pc] = mem_read32(gpr[A] + gpr[B] + D);
    else{ fireInterrupt(FAULT); return; }
    break;

  case 0x3:
    taken = false;
    cond  = M & 0x7;
    indir = (M & 0x8) != 0;

    switch(cond){
    case 0: taken = true; break;
    case 1: taken = ((int32_t)gpr[B] == (int32_t)gpr[C]); break;
    case 2: taken = ((int32_t)gpr[B] != (int32_t)gpr[C]); break;
    case 3: taken = ((int32_t)gpr[B] >  (int32_t)gpr[C]); break;
    default: fireInterrupt(FAULT); return;
    }

    if(taken){
      target = indir ? mem_read32(gpr[A] + D) : gpr[A] + D;
      gpr[pc] = target;
    }
    break;

  case 0x4:
    tmp = gpr[B];
    gpr[B] = gpr[C];
    gpr[C] = tmp;
    break;

  case 0x5:
    switch(M){
    case 0: gpr[A] = gpr[B] + gpr[C]; break;
    case 1: gpr[A] = gpr[B] - gpr[C]; break;
    case 2: gpr[A] = gpr[B] * gpr[C]; break;
    case 3:
      if(gpr[C] == 0){ fireInterrupt(FAULT); return; }
      gpr[A] = (int32_t)gpr[B] / (int32_t)gpr[C];
      break;
    default: fireInterrupt(FAULT); return;
    }
    break;

  case 0x6:
    switch(M){
    case 0: gpr[A] =  ~gpr[B]; break;
    case 1: gpr[A] = gpr[B] &  gpr[C]; break;
    case 2: gpr[A] = gpr[B] |  gpr[C]; break;
    case 3: gpr[A] = gpr[B] ^  gpr[C]; break;
    default: fireInterrupt(FAULT); return;
    }
    break;

  case 0x7:
    switch(M){
    case 0: gpr[A] = gpr[B] << gpr[C]; break;
    case 1: gpr[A] = gpr[B] >> gpr[C]; break;
    default: fireInterrupt(FAULT); return;
    }
    break;

  case 0x8:
    switch(M){
    case 0: mem_write32(gpr[A] + gpr[B] + D, gpr[C]); break;
    case 1: gpr[A] += D; mem_write32(gpr[A], gpr[C]); break;
    case 2: mem_write32(mem_read32(gpr[A] + gpr[B] + D), gpr[C]); break;
    default: fireInterrupt(FAULT); return;
    }
    break;

  case 0x9:
    switch(M){
    case 0: gpr[A] = csr[B]; break;
    case 1: gpr[A] = gpr[B] + D; break;
    case 2: gpr[A] = mem_read32(gpr[B] + gpr[C] + D); break;
    case 3: gpr[A] = mem_read32(gpr[B]); gpr[B] += D; break;
    case 4: csr[A] = gpr[B]; break;
    case 5: csr[A] = csr[B] | D; break;
    case 6: csr[A] = mem_read32(gpr[B] + gpr[C] + D); break;
    case 7: csr[A] = mem_read32(gpr[B]); gpr[B] += D; break;
    default: fireInterrupt(FAULT); return;
    }
    break;

  default:
    fireInterrupt(FAULT);
    return;
  }

  gpr[0] = 0;
};

void S_CS::fireInterrupt(Cause cause){
#ifdef DEBUG_EMU
  fprintf(output, ">>> INT cause=%d pc=0x%08X handler_csr=0x%08X\n",
    cause, gpr[15], csr[1]);
    
  f = 2000000-10000;
#endif
  push32(gpr[pc]);
  push32(csr[status]);
  csr[Reg::cause] = cause;
  csr[status] |= Global;
  gpr[pc] = csr[handler];
  gpr[0] = 0;
};

  // S_CS terminal peripheral

void* S_CS::terminal_fn(void* arg){
  S_CS* s_cs = (S_CS*) arg;

  while(s_cs->running){
    unsigned char c;
    ssize_t n = read(0, &c, 1);
    if(n <= 0) break;
    if(!s_cs->running) break;
    pthread_mutex_lock(&s_cs->interrupt_mutex);
    s_cs->terminal_char = (uint8_t)c;
    s_cs->pending_terminal = true;
    pthread_mutex_unlock(&s_cs->interrupt_mutex);
  }
};

  // S_CS timer peripheral

void* S_CS::timer_fn(void* arg){
  S_CS* s_cs = (S_CS*) arg;
  int periods[] = {500, 1000, 1500, 2000, 5000, 10000, 30000, 60000};

  while(s_cs->running){
    pthread_mutex_lock(&s_cs->interrupt_mutex);
    struct timespec ts = {periods[s_cs->timer_cfg] / 1000, (periods[s_cs->timer_cfg] % 1000)*1000000};
    pthread_mutex_unlock(&s_cs->interrupt_mutex);
    nanosleep(&ts, NULL);
    if(!s_cs->running) break;
    pthread_mutex_lock(&s_cs->interrupt_mutex);
    s_cs->pending_timer = true;
    pthread_mutex_unlock(&s_cs->interrupt_mutex);
  }
};

void S_CS::set_raw_terminal(){
  tcgetattr(0, &old_termios);

  struct termios raw = old_termios;
  cfmakeraw(&raw);
  raw.c_cc[VMIN]  = 1;
  raw.c_cc[VTIME] = 0;

  tcsetattr(0, TCSANOW, &raw);
};

void S_CS::restore_terminal(){
  tcsetattr(0, TCSANOW, &old_termios);
};
