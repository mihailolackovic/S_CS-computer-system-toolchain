#include "../../inc/S_Obj_dump.h"

int readO(FILE* input, FILE* output);
int readHEX(FILE* input, FILE* output);

int main(int argc, const char** argv){
  FILE* input = NULL;
  FILE* output = NULL;
  int inputIndex = -1;

  for(int i = 1; i < argc; i++){
    if(strcmp(argv[i], "-o") == 0){
      if(argv[++i] != NULL)
        output = fopen(argv[i], "w+");
    }
    if(strcmp(argv[i], ">") == 0){
      if(argv[++i] != NULL)
        output = fopen(argv[i], "w+");
    }
    if(strcmp(argv[i], "<") == 0){
      if(input != NULL) goto mul_inputs;
      if(argv[++i] != NULL)
        input = fopen(argv[i], "r+");
      inputIndex = i;
    } else {
      if(input != NULL) goto mul_inputs;
      input = fopen(argv[i], "r+");
      inputIndex = i;
    }
  }

  if(input == NULL) goto no_input;
  if(output == NULL){
    char outputName[strlen(argv[inputIndex])+5];
    strcpy(outputName, argv[inputIndex]);
    strcat(outputName, ".txt");
    output = fopen(outputName, "w+");
  }

  int sectionTableHeader = 0;
  unsigned long signature = 0x0;
  fseek(input, 0x0, SEEK_SET);
  fread(&signature, 8, 1, input);

  if(signature != 0x3436464C455F5300) goto notS_ELF;

  fseek(input, 0xC, SEEK_SET);
  fread(&sectionTableHeader, 4, 1, input);

  if(sectionTableHeader != 0) readO(input, output);
  else readHEX(input, output);

  fclose(input);
  fclose(output);
  exit(0);

  no_input:
  printf("\n\nS_OBj_dump:   ERROR NO INPUT FILE!\n\n");
  exit(1);
  mul_inputs:
  printf("\n\nS_OBj_dump:   ERROR MULTIPLE INPUT FILES!\n\n");
  exit(2);
  notS_ELF:
  printf("\n\nS_OBj_dump:   ERROR FILE NOT AN S_ELF BINARY!\n\n");
  exit(3);
}

int readO(FILE* input, FILE* output){
  fseek(input, 0x0, SEEK_END);
  int fileSize = ftell(input);

  int sectionHeaderOffset;
  fseek(input, 0x0C, SEEK_SET);
  fread(&sectionHeaderOffset, 4, 1, input);

  fprintf(output,"Signature: \" S_ELF64\"\nSegment Header Offset: 0 (Relocatable .o file)\nSection Table Offset: %d\n", sectionHeaderOffset);

  int sectionPoolSize;
  fseek(input, sectionHeaderOffset, SEEK_SET);
  fread(&sectionPoolSize, 4, 1, input);

  int sectionPoolStart = sectionHeaderOffset - sectionPoolSize;
  int sectionTableSize = fileSize - sectionHeaderOffset - 4;
  int sectionEntryCount = sectionTableSize / 12;

  char* sectionPool = malloc(sectionPoolSize*sizeof(char));
  fseek(input, sectionPoolStart, SEEK_SET);
  fread(sectionPool, 1, sectionPoolSize, input);

  Section* sectionTable = malloc((sectionEntryCount+1)*sizeof(Section));

  fseek(input, sectionHeaderOffset+4, SEEK_SET);
  for(int i = 0; i < sectionEntryCount; i++){
    int sectionNameOffset;
    fread(&sectionNameOffset, 4, 1, input);
    sectionTable[i].name = strdup(sectionPool+sectionNameOffset);
    fread(&sectionTable[i].lenght, 4, 1, input);
    fread(&sectionTable[i].base, 4, 1, input);
  }

  int symbolTablePoolSize = sectionTable[sectionEntryCount-4].lenght;
  int symbolTablePoolOffset = sectionTable[sectionEntryCount-4].base;
  int symbolTableSize = sectionTable[sectionEntryCount-3].lenght;
  int symbolTableOffset = sectionTable[sectionEntryCount-3].base;
  int symbolTableEntryCount = symbolTableSize / 16;
  int relocTableSize = sectionTable[sectionEntryCount-2].lenght;
  int relocTableOffset = sectionTable[sectionEntryCount-2].base;
  int relocTableEntryCount = relocTableSize / 20;

  char* symbolPool = malloc(symbolTablePoolSize*sizeof(char));
  fseek(input, symbolTablePoolOffset, SEEK_SET);
  fread(symbolPool, 1, symbolTablePoolSize, input);

  Symbol* symbolTable = malloc((symbolTableEntryCount+1)*sizeof(Symbol));

  fseek(input, symbolTableOffset, SEEK_SET);
  for(int i = 0; i < symbolTableEntryCount; i++){
    int symbolNameOffset;
    fread(&symbolNameOffset, 4, 1, input);
    symbolTable[i].name = strdup(symbolPool+symbolNameOffset);
    fread(&symbolTable[i].value, 4, 1, input);
    fread(&symbolTable[i].size, 4, 1, input);
    fread(&symbolTable[i].type, 1, 1, input);
    symbolTable[i].type = (char)symbolTable[i].type;
    fread(&symbolTable[i].binding, 1, 1, input);
    symbolTable[i].binding = (char)symbolTable[i].binding;
    fread(&symbolTable[i].section, 2, 1, input);
    symbolTable[i].section = (short)symbolTable[i].section;
  }

  Relocation* relocTable = malloc((relocTableEntryCount+1)*sizeof(Relocation));

  fseek(input, relocTableOffset, SEEK_SET);
  for(int i = 0; i < relocTableEntryCount; i++){
    fread(&relocTable[i].section, 4, 1, input);
    fread(&relocTable[i].symbol, 4, 1, input);
    fread(&relocTable[i].offset, 4, 1, input);
    fread(&relocTable[i].addend, 4, 1, input);
    fread(&relocTable[i].type, 1, 1, input);
    relocTable[i].type = (char)relocTable[i].type;
    fseek(input, 3, SEEK_CUR);
  }

  for(int i = 0; i < sectionEntryCount-4; i++){
    fprintf(output, "\n\nSection %s:\n\n", sectionTable[i].name);
    int LC = 0;
    while(LC < sectionTable[i].lenght){
      int sym_idx = -1;
      for(int s = 0; s < symbolTableEntryCount; s++){
        if(symbolTable[s].section == i && symbolTable[s].value == LC && (symbolTable[s].type == DATA || symbolTable[s].type == STR)){
          sym_idx = s;
          break;
        }
      }
      if(sym_idx != -1){
        int sym_size = symbolTable[sym_idx].size;
        char* type_str = (symbolTable[sym_idx].type == STR) ? "STR" : "DATA";
        fprintf(output, "0x%08X:  <%s %s>  ", LC, type_str, symbolTable[sym_idx].name);
        fseek(input, sectionTable[i].base + LC, SEEK_SET);
        for(int b = 0; b < sym_size; b++){
          uint8_t byte; fread(&byte, 1, 1, input);
          fprintf(output, "%02X ", byte);
        }
        if(symbolTable[sym_idx].type == STR){
          fseek(input, sectionTable[i].base + LC, SEEK_SET);
          fprintf(output, " |  \"");
          for(int b = 0; b < sym_size; b++){
            uint8_t byte; fread(&byte, 1, 1, input);
            fprintf(output, "%c", (byte >= 32 && byte < 127) ? byte : '.');
          }
          fprintf(output, "\"");
        }
        fprintf(output, "\n");
        LC += sym_size;
      } else {
        if(LC + 4 > sectionTable[i].lenght) break;
        unsigned int instruction;
        fseek(input, sectionTable[i].base + LC, SEEK_SET);
        fread(&instruction, 4, 1, input);
        char* relsym = NULL;
        for(int r = 0; r < relocTableEntryCount; r++){
          if(relocTable[r].section == i && relocTable[r].offset == LC){
            relsym = symbolTable[relocTable[r].symbol].name;
            break;
          }
        }
        if(relsym)
          fprintf(output, "0x%08X:   %-40s ; <%s>\n", LC, instructionPrinter(instruction), relsym);
        else
          fprintf(output, "0x%08X:   %s\n", LC, instructionPrinter(instruction));
        LC += 4;
      }
    }
  }

  fprintf(output, "\n\nSymbol Table:\n\nNAME | VALUE  |  SIZE  |  TYPE  |  BINDING  |  SECTION\n\n");
  char* type;
  char* section;
  char* bind;
  for(int i = 0; i < symbolTableEntryCount; i++){
    switch(symbolTable[i].section){
    case -2: section = "EXTERN"; break;
    case -1: section = "UND"; break;
    case SECTION_TABLE_LENGHT: section = "*ABS*"; break;
    default: section = sectionTable[symbolTable[i].section].name; break;
    }
    switch(symbolTable[i].type){
    case NOTYPE: type = "NOTYPE"; break;
    case DATA: type = "DATA"; break;
    case FUNC: type = "FUNC"; break;
    case SECT: type = "SECT"; break;
    case SRC: type = "SRC"; break;
    case STR: type = "STR"; break;
    default: type = "?"; break;
    }
    switch(symbolTable[i].binding){
    case LOCAL:  bind = "LOCAL";  break;
    case GLOBAL: bind = "GLOBAL"; break;
    case EXTERN: bind = "EXTERN"; break;
    default:     bind = "?";      break;
    }
    fprintf(output, "%s  |  %d  |  %d  |  %s  | %s  |  %s\n",
      symbolTable[i].name, symbolTable[i].value, symbolTable[i].size, type, bind, section);
  }

  fprintf(output, "\n\nReloc Table:\n\nSECTION  |  SYMBOL  |  TYPE  |  OFFSET  | ADDEND\n\n");
  for(int i = 0; i < relocTableEntryCount; i++){
    char* rtype;
    switch(relocTable[i].type){
    case UNINITIALISED: goto reloc_done;
    case R_ABS_12: rtype = "R_ABS_12"; break;
    case R_PC_12: rtype = "R_PC_12"; break;
    case R_ABS_32: rtype = "R_ABS_32"; break;
    case R_PC_32: rtype = "R_PC_32"; break;
    case RESOLVED: rtype = "RESOLVED"; break;
    default: goto reloc_done;
    }
    fprintf(output, "%s  |  %s  |  %s  |  %d  |  %d\n",
      sectionTable[relocTable[i].section].name,
      symbolTable[relocTable[i].symbol].name,
      rtype, relocTable[i].offset, relocTable[i].addend);
  }
  reloc_done:;

  fprintf(output, "\n\nSection Table:\n\nNAME | LENGHT  |  OFFSET\n\n");
  for(int i = 0; i < sectionEntryCount; i++){
    fprintf(output, "%s  |  %d  |  %d\n", sectionTable[i].name,
      (i==sectionEntryCount-1)?sectionPoolSize:sectionTable[i].lenght,
      sectionTable[i].base);
  }
  return 0;
}

int readHEX(FILE* input, FILE* output){
  fseek(input, 0x0, SEEK_END);
  int fileSize = ftell(input);

  int segmentTableOffset;
  fseek(input, 0x08, SEEK_SET);
  fread(&segmentTableOffset, 4, 1, input);

  fprintf(output, "Signature: \" S_ELF64\"\nSegment Table Offset: %d\nSection Table Offset: 0 (Executable)\n", segmentTableOffset);

  fseek(input, segmentTableOffset, SEEK_SET);
  int segmentCount;
  fread(&segmentCount, 4, 1, input);

  typedef struct { int load_address; int size; int file_offset; } Segment;
  Segment* segments = malloc(segmentCount * sizeof(Segment));

  fprintf(output, "\n\nSegment Table:\n\nLOAD_ADDRESS  |  SIZE  |  FILE_OFFSET\n\n");
  for(int i = 0; i < segmentCount; i++){
    fread(&segments[i].load_address, 4, 1, input);
    fread(&segments[i].size, 4, 1, input);
    fread(&segments[i].file_offset, 4, 1, input);
    char* label = "";
    if(segments[i].load_address == (int)0xFFFFFFFF) label = "  [symbol metadata]";
    if(segments[i].load_address == (int)0xFFFFFFFE) label = "  [sym_table slots]";
    fprintf(output, "0x%08X  |  %d  |  %d%s\n",
      segments[i].load_address, segments[i].size, segments[i].file_offset, label);
  }

  HexSym* syms = NULL;
  int sym_count = 0;

  for(int i = 0; i < segmentCount; i++){
    if(segments[i].load_address != (int)0xFFFFFFFF) continue;
    syms = malloc(256 * sizeof(HexSym));
    fseek(input, segments[i].file_offset, SEEK_SET);
    int bytes_read = 0;
    while(bytes_read < segments[i].size){
      int address;
      int size;
      uint8_t type;
      uint8_t binding;
      uint16_t name_len;
      fread(&address, 4, 1, input);
      fread(&size, 4, 1, input);
      fread(&type, 1, 1, input);
      fread(&binding, 1, 1, input);
      fread(&name_len, 2, 1, input);
      char* name = malloc(name_len + 1);
      fread(name, 1, name_len, input);
      name[name_len] = '\0';
      syms[sym_count++] = (HexSym){address, size, type, binding, name};
      bytes_read += 4 + 4 + 1 + 1 + 2 + name_len;
    }
    break;
  }

  fprintf(output, "\n\nSegment Contents:\n\n");

  for(int i = 0; i < segmentCount; i++){
    if(segments[i].load_address == (int)0xFFFFFFFF) continue;

    if(segments[i].load_address == (int)0xFFFFFFFE){
      fprintf(output, "\nSym_table Slots (size=%d):\n\n", segments[i].size);
      int slot_count = segments[i].size / 4;
      for(int s = 0; s < slot_count; s++){
        uint32_t val;
        fseek(input, segments[i].file_offset + s*4, SEEK_SET);
        fread(&val, 4, 1, input);
        char* matched = NULL;
        for(int k = 0; k < sym_count; k++){
          if((uint32_t)syms[k].address == val){ matched = syms[k].name; break; }
        }
        if(matched) fprintf(output, "  slot[%d]:  0x%08X  ; <%s>\n", s, val, matched);
        else fprintf(output, "  slot[%d]:  0x%08X\n", s, val);
      }
      continue;
    }

    fprintf(output, "\nSegment %d (load=0x%08X, size=%d):\n\n", i, segments[i].load_address, segments[i].size);

    int LC = 0;
    while(LC < segments[i].size){
      int current_addr = segments[i].load_address + LC;
      int sym_idx = -1;
      for(int s = 0; s < sym_count; s++){
        if(current_addr >= syms[s].address && current_addr < syms[s].address + syms[s].size){
          sym_idx = s;
          break;
        }
      }
      if(sym_idx != -1 && current_addr == syms[sym_idx].address){
        int sym_size = syms[sym_idx].size;
        char* type_str = (syms[sym_idx].type == STR) ? "STR" : "DATA";
        if(syms[sym_idx].type == STR)
          fprintf(output, "0x%08X:  <%s>  ", current_addr, type_str);
        else
          fprintf(output, "0x%08X:  <%s %s>  ", current_addr, type_str, syms[sym_idx].name);
        fseek(input, segments[i].file_offset + LC, SEEK_SET);
        for(int b = 0; b < sym_size; b++){
          uint8_t byte; fread(&byte, 1, 1, input);
          fprintf(output, "%02X ", byte);
        }
        if(syms[sym_idx].type == STR){
          fseek(input, segments[i].file_offset + LC, SEEK_SET);
          fprintf(output, " |  \"");
          for(int b = 0; b < sym_size; b++){
            uint8_t byte; fread(&byte, 1, 1, input);
            fprintf(output, "%c", (byte >= 32 && byte < 127) ? byte : '.');
          }
          fprintf(output, "\"");
        }
        fprintf(output, "\n");
        LC += sym_size;
      } else {
        if(LC + 4 > segments[i].size) break;
        unsigned int instruction;
        fseek(input, segments[i].file_offset + LC, SEEK_SET);
        fread(&instruction, 4, 1, input);
        uint8_t oc = (instruction >> 28) & 0xF;
        uint8_t m = (instruction >> 24) & 0xF;
        uint8_t b = (instruction >> 16) & 0xF;
        int32_t d = instruction & 0xFFF;
        d = (d << 20) >> 20;
        char annot[128] = "";
        int is_pc_slot = (b == 15) && ((oc == 0x9 && m == 2) || (oc == 0x8 && m == 2) || (oc == 0x2 && m == 1) || (oc == 0x3 && (m & 0x8)));
        if(is_pc_slot){
          int slot_addr = current_addr + 4 + d;
          for(int sgi = 0; sgi < segmentCount; sgi++){
            if(slot_addr >= segments[sgi].load_address && slot_addr < segments[sgi].load_address + segments[sgi].size){
              uint32_t slotval;
              long save = ftell(input);
              fseek(input, segments[sgi].file_offset + (slot_addr - segments[sgi].load_address), SEEK_SET);
              fread(&slotval, 4, 1, input);
              fseek(input, save, SEEK_SET);
              char* matched = NULL;
              for(int s = 0; s < sym_count; s++){
                if((uint32_t)syms[s].address == slotval){ matched = syms[s].name; break; }
              }
              if(matched) snprintf(annot, 128, "  ; <%s = 0x%08X>", matched, slotval);
              else snprintf(annot, 128, "  ; <0x%08X>", slotval);
              break;
            }
          }
        }
        fprintf(output, "0x%08X:  %08X  %s%s\n", current_addr, instruction, instructionPrinter(instruction), annot);
        LC += 4;
      }
    }
  }

  if(syms){
    fprintf(output, "\n\nSymbol Metadata:\n\nADDRESS  |  SIZE  |  TYPE  |  NAME\n\n");
    for(int i = 0; i < sym_count; i++){
      char* type_str = (syms[i].type == STR) ? "STR" : "DATA";
      fprintf(output, "0x%08X  |  %d  |  %s  |  %s\n", syms[i].address, syms[i].size, type_str, syms[i].name);
      free(syms[i].name);
    }
    free(syms);
  }

  free(segments);
  return 0;
}

char* gprtos(uint8_t reg){
  if(reg == 14) return "%sp";
  if(reg == 15) return "%pc";
  static char buf[8][5];
  static int idx = 0;
  idx = (idx + 1) % 8;
  snprintf(buf[idx], 5, "%%r%d", reg);
  return buf[idx];
}

char* csrtos(uint8_t reg){
  if(reg == 0) return "%status";
  if(reg == 1) return "%handler";
  if(reg == 2) return "%cause";
  return "%csr?";
}

char* instructionPrinter(unsigned int instruction){
  static char buf[128];
  uint8_t OC = (instruction >> 28) & 0xF;
  uint8_t M = (instruction >> 24) & 0xF;
  uint8_t A = (instruction >> 20) & 0xF;
  uint8_t B = (instruction >> 16) & 0xF;
  uint8_t C = (instruction >> 12) & 0xF;
  int32_t D = instruction & 0xFFF;
  D = (D << 20) >> 20;

  switch(OC){
  case 0: return (M == 0) ? "halt" : "UNKNOWN";
  case 1: return (M == 0) ? "int" : "UNKNOWN";
  case 2:
    if(M == 0x0) snprintf(buf, 128, "call  %s+%s+%d", gprtos(A), gprtos(B), D);
    else if(M==0x1) snprintf(buf, 128, "call  mem[%s+%s+%d]", gprtos(A), gprtos(B), D);
    else snprintf(buf, 128, "call  mod=0x%X A=%s B=%s D=%d", M, gprtos(A), gprtos(B), D);
    return buf;
  case 3:
    {
      int indirect = (M & 0x8) != 0;
      int cond = M & 0x7;
      char* cond_str[] = {"jmp","beq","bne","bgt"};
      char tgt[64];
      if(indirect) snprintf(tgt, 64, "mem[%s+%d]", gprtos(A), D);
      else snprintf(tgt, 64, "%s+%d", gprtos(A), D);
      if(cond == 0) snprintf(buf, 128, "%-3s   pc<=%s", cond_str[cond], tgt);
      else snprintf(buf, 128, "%-3s   %s,%s  pc<=%s", cond_str[cond], gprtos(B), gprtos(C), tgt);
    }
    return buf;
  case 4:
    snprintf(buf, 128, "xchg  %s,%s", gprtos(B), gprtos(C));
    return buf;
  case 5:
    { char* ops[] = {"add","sub","mul","div"};
      char* opc[] = {"+","-","*","/"};
      if(M > 3) return "UNKNOWN";
      snprintf(buf, 128, "%-3s   %s<=%s%s%s", ops[M], gprtos(A), gprtos(B), opc[M], gprtos(C)); }
    return buf;
  case 6:
    { char* ops[] = {"not","and","or","xor"};
      if(M > 3) return "UNKNOWN";
      if(M == 0) snprintf(buf, 128, "not   %s<=~%s", gprtos(A), gprtos(B));
      else snprintf(buf, 128, "%-3s   %s<=%s op %s", ops[M], gprtos(A), gprtos(B), gprtos(C)); }
    return buf;
  case 7:
    { char* ops[] = {"shl","shr"};
      if(M > 1) return "UNKNOWN";
      snprintf(buf, 128, "%-3s   %s<=%s %s %s", ops[M], gprtos(A), gprtos(B), ops[M], gprtos(C)); }
    return buf;
  case 8:
    if(M == 0) snprintf(buf, 128, "st    mem[%s+%s+%d]<=%s", gprtos(A), gprtos(B), D, gprtos(C));
    else if(M==1) snprintf(buf, 128, "st    %s+=%d; mem[%s]<=%s", gprtos(A), D, gprtos(A), gprtos(C));
    else if(M==2) snprintf(buf, 128, "st    mem[mem[%s+%s+%d]]<=%s", gprtos(A), gprtos(B), D, gprtos(C));
    else snprintf(buf, 128, "st    mod=0x%X", M);
    return buf;
  case 9:
    if(M==0) snprintf(buf, 128, "ld    %s<=csr[%s]", gprtos(A), csrtos(B));
    else if(M==1) snprintf(buf, 128, "ld    %s<=%s+%d", gprtos(A), gprtos(B), D);
    else if(M==2) snprintf(buf, 128, "ld    %s<=mem[%s+%s+%d]", gprtos(A), gprtos(B), gprtos(C), D);
    else if(M==3) snprintf(buf, 128, "ld    %s<=mem[%s]; %s+=%d", gprtos(A), gprtos(B), gprtos(B), D);
    else if(M==4) snprintf(buf, 128, "csrwr csr[%s]<=%s", csrtos(A), gprtos(B));
    else if(M==5) snprintf(buf, 128, "csrwr csr[%s]<=csr[%s]|%d", csrtos(A), csrtos(B), D);
    else if(M==6) snprintf(buf, 128, "csrwr csr[%s]<=mem[%s+%s+%d]", csrtos(A), gprtos(B), gprtos(C), D);
    else if(M==7) snprintf(buf, 128, "csrwr csr[%s]<=mem[%s]; %s+=%d", csrtos(A), gprtos(B), gprtos(B), D);
    else snprintf(buf, 128, "ld    mod=0x%X", M);
    return buf;
  }
  return "UNKNOWN 4BYTE";
}