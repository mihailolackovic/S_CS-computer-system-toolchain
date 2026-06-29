#include "../../inc/Linker/Linker_functions.h"

//#define DEBUG_LINK

void extractInformation(Linker_init* lI, InputFile* iF){
  for(int i = 0; i < lI->input_count; i++){
    int sectionHeaderTableOffset = 0;
    int sectionTable_Pool_size = 0;
    int sectionCount = 0;

    fseek(lI->input[i], 0, SEEK_END);
    int total_file_size = ftell(lI->input[i]);

    fseek(lI->input[i], 0xC, SEEK_SET);
    fread(&sectionHeaderTableOffset, 4, 1, lI->input[i]);
    
    if(sectionHeaderTableOffset == 0){
      iF[i].error = FILE_BROKEN;
      return;
    }

    fseek(lI->input[i], sectionHeaderTableOffset, SEEK_SET);
    fread(&sectionTable_Pool_size, 4, 1, lI->input[i]);

    long dynamic_table_bytes = total_file_size - sectionHeaderTableOffset - 4;
    sectionCount = dynamic_table_bytes / 12;

    if(sectionCount <= 0 || (dynamic_table_bytes % 12 >= 4)){
      iF[i].error = FILE_BROKEN;
      return;
    }

    iF[i].sectionTable_count = sectionCount;
    iF[i].error = LINKER_PASS;
    iF[i].sectionTable = malloc(sectionCount * sizeof(Section));

    char* string_pool = malloc(sectionTable_Pool_size);
    if(string_pool == NULL){
      iF[i].error = MALLOC_FAILED;
      return;
    }
    long string_pool_start = sectionHeaderTableOffset - sectionTable_Pool_size;
    
    fseek(lI->input[i], string_pool_start, SEEK_SET);
    fread(string_pool, 1, sectionTable_Pool_size, lI->input[i]);
    
    fseek(lI->input[i], sectionHeaderTableOffset+4, SEEK_SET);

    for(int j = 0; j < sectionCount; j++){
      int name_offset = 0;
      Section section;
      fread(&name_offset, 4, 1, lI->input[i]);
      section.name = strdup(string_pool + name_offset);
      fread(&section.lenght, 4, 1, lI->input[i]);
      fread(&section.base, 4, 1, lI->input[i]);
      iF[i].sectionTable[j] = section;
    }

    int symbolTable_offset = iF[i].sectionTable[sectionCount-3].base;
    int symbolTable_size = iF[i].sectionTable[sectionCount-3].lenght;
    int symbolTable_Pool_size = iF[i].sectionTable[sectionCount-4].lenght;
    string_pool_start = iF[i].sectionTable[sectionCount-4].base;

    int symbol_count = symbolTable_size / 16;
    iF[i].symbolTable = malloc(symbol_count * sizeof(Symbol));

    string_pool = malloc(symbolTable_size);
    fseek(lI->input[i], string_pool_start, SEEK_SET);
    fread(string_pool, 1, symbolTable_Pool_size, lI->input[i]);
    
    fseek(lI->input[i], symbolTable_offset, SEEK_SET);
    for(int j = 0; j < symbol_count; j++){
      int name_offset = 0;
      Symbol symbol;
      fread(&name_offset, 4, 1, lI->input[i]);
      symbol.name = strdup(string_pool + name_offset);
      fread(&symbol.value, 4, 1, lI->input[i]);
      fread(&symbol.size, 4, 1, lI->input[i]);
      fread(&symbol.type, 1, 1, lI->input[i]);
      symbol.type = (char)symbol.type;
      fread(&symbol.binding, 1, 1, lI->input[i]);
      symbol.binding = (char)symbol.binding;
      fread(&symbol.section, 2, 1, lI->input[i]);
      symbol.section = (short)symbol.section;
      iF[i].symbolTable[j] = symbol;
    }
    iF[i].symbolTable_count = symbol_count;

    int relocTable_offset = iF[i].sectionTable[sectionCount-2].base;
    int relocTable_size = iF[i].sectionTable[sectionCount-2].lenght;
    int reloc_count = relocTable_size / 20;
    iF[i].relocTable = malloc(reloc_count * sizeof(Relocation));

    for(int j = 0; j < reloc_count; j++){
      Relocation relocation;
      fread(&relocation.section, 4, 1, lI->input[i]);
      fread(&relocation.symbol, 4, 1, lI->input[i]);
      fread(&relocation.offset, 4, 1, lI->input[i]);
      fread(&relocation.addend, 4, 1, lI->input[i]);
      fread(&relocation.type, 1, 1, lI->input[i]);
      fseek(lI->input[i], 3, SEEK_CUR);
      iF[i].relocTable[j] = relocation;
    }
    iF[i].relocTable_count = reloc_count;
    iF[i].file_name = strdup(lI->inputName[i]);
  }
};

int createMergedSectionTable(Linker_init* lI, InputFile* iF, MergedSection* mergedSectionTable){
  int mergedSectionTableSize = 0;
  for(int i = 0; i < lI->input_count; i++){
    for(int j = 0; j < iF[i].sectionTable_count; j++){
      Section section = iF[i].sectionTable[j];

      if(strcmp(section.name, "Sect_Pool")==0) continue;
      if(strcmp(section.name, "Sym_table")==0) continue;
      if(strcmp(section.name, "Sym_pool")==0) continue;
      if(strcmp(section.name, "Rel_Table")==0) continue;

      int index = findMergedSection(section.name, mergedSectionTable, mergedSectionTableSize);

      unsigned char* content = malloc(section.lenght);
      fseek(lI->input[i], section.base, SEEK_SET);
      fread(content, 1, section.lenght, lI->input[i]);

      if(index == -1){
        int* fileArray   = malloc(lI->input_count*sizeof(int));
        int* sectionArray = malloc(lI->input_count*sizeof(int));
        int* offsetArray  = malloc(lI->input_count*sizeof(int));
        fileArray[0]   = i;
        sectionArray[0] = j;
        offsetArray[0]  = 0;
        int file_count  = 1;
        mergedSectionTable[mergedSectionTableSize++] =
          (MergedSection){strdup(section.name), 0, section.lenght, fileArray, sectionArray, offsetArray, file_count, 0, content};
      } else {
        MergedSection* m = &mergedSectionTable[index];
        m->file_count++;
        m->filesArray[m->file_count-1]   = i;
        m->sectionArray[m->file_count-1] = j;
        m->offsetArray[m->file_count-1]  = m->size;
        m->content = realloc(m->content, m->size + section.lenght);
        memcpy(m->content + m->size, content, section.lenght);
        m->size += section.lenght;
        free(content);
      }
    }
  }
  return mergedSectionTableSize;
};

int createSectionMap(Linker_init* lI, InputFile* iF, MergedSection* mergedSectionTable, SectionMap* sectionMap, int sectionTableSize){
  if(lI->place_count == 0){
    lI->place_count = 1;
    lI->place = malloc(sizeof(Place));
    lI->place[0] = (Place){mergedSectionTable[0].name, 0x40000000};
  }
  int sectionMapSize = 0;

  for(int p = 0; p < lI->place_count; p++){
    for(int i = 0; i < sectionTableSize; i++){
      if(strcmp(mergedSectionTable[i].name, lI->place[p].section) == 0){
        MergedSection* m = &mergedSectionTable[i];
        int base = (int)lI->place[p].address;
        m->start  = base;
        m->placed = 1;
        for(int j = 0; j < m->file_count; j++){
          int file = m->filesArray[j];
          int section = m->sectionArray[j];
          int addr = base + m->offsetArray[j];
          addSectionMap(file, section, i, addr, sectionMap);
          sectionMapSize++;
        }
        break;
      }
    }
  }

  int adr_curr = 0;
  for(int i = 0; i < sectionTableSize; i++){
    if(mergedSectionTable[i].placed){
      int end = mergedSectionTable[i].start + mergedSectionTable[i].size;
      if(end > adr_curr) adr_curr = end;
    }
  }

  for(int i = 0; i < sectionTableSize; i++){
    MergedSection* m = &mergedSectionTable[i];
    if(m->placed) continue;
    m->start = adr_curr;
    m->placed = 1;

    uint32_t sec_end = (uint32_t)adr_curr + (uint32_t)m->size;
    if(sec_end < (uint32_t)adr_curr ||  (uint32_t)sec_end > 0xFFFFFF00u){
      printf( "LINKER ERROR: section '%s' placed at 0x%X overflows \
          into reserved/MMIO region\n", m->name, adr_curr);
      return -1;
    }

    for(int j = 0; j < m->file_count; j++){
      int file = m->filesArray[j];
      int section = m->sectionArray[j];
      int addr = adr_curr + m->offsetArray[j];
      addSectionMap(file, section, i, addr, sectionMap);
      sectionMapSize++;
    }
    adr_curr += m->size;
  }
#ifdef DEBUG_LINK
  for(int i = 0; i < sectionTableSize; i++){
    fprintf(stderr, "SECTMAP: '%s' start=0x%X size=%d placed=%d\n",
      mergedSectionTable[i].name, mergedSectionTable[i].start,
      mergedSectionTable[i].size, mergedSectionTable[i].placed);
  }
#endif
  return sectionMapSize;
};

int createGlobalSymbolTable(Linker_init* lI, InputFile* iF, SectionMap* sectionMap, int sectionMapSize, GlobalSymbol* globalSymbolTable){
  int count = 0;

  for(int i = 0; i < lI->input_count; i++){
    for(int s = 0; s < iF[i].symbolTable_count; s++){
      Symbol sym = iF[i].symbolTable[s];

      if(sym.binding != GLOBAL) continue;
      if(sym.type == SECT || sym.type == SRC) continue;

      int address;
      if(sym.section == ABS_S){
        address = sym.value;
      } else if(sym.section == UND_S || sym.section == EXTERN_S){
        continue;
      } else {
        int idx = findSectionMap(i, sym.section, sectionMap, sectionMapSize);
        address = sectionMap[idx].offset + sym.value;
      }

      if(findGlobalSymbol(sym.name, globalSymbolTable, count) != -1){
        iF[i].error = LINK_ERROR_MULTIPLE_DEFINITION;
        iF[i].symbolTable_count = s;
        continue;
      }

      globalSymbolTable[count++] = (GlobalSymbol){strdup(sym.name), address};
    }
  }

  for(int i = 0; i < lI->input_count; i++){
    for(int s = 0; s < iF[i].symbolTable_count; s++){
      Symbol sym = iF[i].symbolTable[s];
      if(sym.binding != EXTERN) continue;
      if(findGlobalSymbol(sym.name, globalSymbolTable, count) == -1){
        iF[i].error = LINK_ERROR_UNDEFINED_SYMBOL;
      }
    }
  }

  return count;
};

int resolveRelocations(
  Linker_init* lI,
  InputFile* iF,
  MergedSection* mergedSectionTable,
  int mergedSectionTableSize,
  SectionMap* sectionMap,
  int sectionMapSize,
  GlobalSymbol* globalSymbolTable,
  int globalSymbolCount)
{
  SymTableEntry symTable[256];
  int symTableSize = 0;
  uint32_t* symTableContent = malloc(256 * sizeof(uint32_t));

  for(int i = 0; i < lI->input_count; i++){
    for(int r = 0; r < iF[i].relocTable_count; r++){
      Relocation reloc = iF[i].relocTable[r];
      if(reloc.type == RESOLVED)     continue;
      if(reloc.type == UNINITIALISED) continue;
      if(reloc.type == R_ABS_32)     continue;

      Symbol sym = iF[i].symbolTable[reloc.symbol];

      int m_idx = findMergedSection(
        iF[i].sectionTable[reloc.section].name,
        mergedSectionTable, mergedSectionTableSize);
      int f_off = 0;
      for(int k = 0; k < mergedSectionTable[m_idx].file_count; k++){
        if(mergedSectionTable[m_idx].filesArray[k] == i &&
          mergedSectionTable[m_idx].sectionArray[k] == reloc.section){
          f_off = mergedSectionTable[m_idx].offsetArray[k];
          break;
        }
      }
      uint32_t ins = *(uint32_t*)(mergedSectionTable[m_idx].content + f_off + reloc.offset);
      int oc = (ins >> 28) & 0xF;
      int is_branch = (oc == 0x2 || oc == 0x3);

      int needs_slot = 0;
      if(sym.section == EXTERN_S || sym.section == UND_S)
        needs_slot = 1;
      else if(sym.section == ABS_S)
        needs_slot = 1;
      else if(sym.section != reloc.section)
        needs_slot = 1;
      else if(reloc.type == R_PC_12 && !is_branch)
        needs_slot = 1;

      if(!needs_slot) continue;

      int address = 0;
      if(sym.section == ABS_S){
        address = sym.value;
      } else if(sym.binding == EXTERN || sym.section == EXTERN_S || sym.section == UND_S){
        int g = findGlobalSymbol(sym.name, globalSymbolTable, globalSymbolCount);
        if(g == -1){ iF[i].error = LINK_ERROR_UNDEFINED_SYMBOL; continue; }
        address = globalSymbolTable[g].value;
      } else {
        int sm = findSectionMap(i, sym.section, sectionMap, sectionMapSize);
        address = sectionMap[sm].offset + sym.value;
      }

      int found = -1;
      for(int k = 0; k < symTableSize; k++){
        if(strcmp(symTable[k].name, sym.name) == 0 &&
           symTableContent[k] == (uint32_t)address){
          found = k;
          break;
        }
      }

      if(found == -1){
        symTable[symTableSize]        = (SymTableEntry){sym.name, symTableSize};
        symTableContent[symTableSize] = (uint32_t)address;
        symTableSize++;
      }
    }
  }

  int sym_table_merged_index = mergedSectionTableSize;
  int sym_table_size = symTableSize * sizeof(uint32_t);
  int* fileArray = malloc(sizeof(int));
  int* secArray  = malloc(sizeof(int));
  int* offArray  = malloc(sizeof(int));
  fileArray[0] = -1; secArray[0] = -1; offArray[0] = 0;
  mergedSectionTable[mergedSectionTableSize++] = (MergedSection){
    strdup("__sym_table__"), 0, sym_table_size,
    fileArray, secArray, offArray, 1, 0,
    (unsigned char*)symTableContent
  };

  int adr_curr = 0;
  for(int i = 0; i < mergedSectionTableSize - 1; i++){
    if(mergedSectionTable[i].placed){
      int end = mergedSectionTable[i].start + mergedSectionTable[i].size;
      if(end > adr_curr) adr_curr = end;
    }
  }
  mergedSectionTable[sym_table_merged_index].start  = adr_curr;
  mergedSectionTable[sym_table_merged_index].placed = 1;
  int sym_table_start = adr_curr;

  for(int i = 0; i < lI->input_count; i++){
    for(int r = 0; r < iF[i].relocTable_count; r++){
      Relocation reloc = iF[i].relocTable[r];
      if(reloc.type == RESOLVED)     continue;
      if(reloc.type == UNINITIALISED) continue;

      Symbol sym = iF[i].symbolTable[reloc.symbol];

      int merged_idx = findMergedSection(
        iF[i].sectionTable[reloc.section].name,
        mergedSectionTable, mergedSectionTableSize);
      MergedSection* m = &mergedSectionTable[merged_idx];

      int file_offset = 0;
      for(int k = 0; k < m->file_count; k++){
        if(m->filesArray[k] == i && m->sectionArray[k] == reloc.section){
          file_offset = m->offsetArray[k];
          break;
        }
      }

      int instruction_address    = m->start + file_offset + reloc.offset + 4;
      int byte_offset_in_content = file_offset + reloc.offset;
      uint32_t* word = (uint32_t*)(m->content + byte_offset_in_content);

      if(reloc.type == R_ABS_32){
        int address = 0;
        if(sym.section == ABS_S){
          address = sym.value;
        } else if(sym.binding == EXTERN || sym.section == EXTERN_S || sym.section == UND_S){
          int g = findGlobalSymbol(sym.name, globalSymbolTable, globalSymbolCount);
          if(g == -1){ iF[i].error = LINK_ERROR_UNDEFINED_SYMBOL; continue; }
          address = globalSymbolTable[g].value;
        } else {
          int sm = findSectionMap(i, sym.section, sectionMap, sectionMapSize);
          address = sectionMap[sm].offset + sym.value;
        }
        *word = (uint32_t)address;
        iF[i].relocTable[r].type = RESOLVED;
        continue;
      }

      if(reloc.type == R_PC_12 || reloc.type == R_ABS_12){
        int slot = -1;
        
        int target_addr = 0;
        int resolvable = 1;
        if(sym.section == ABS_S){
          target_addr = sym.value;
        } else if(sym.binding == EXTERN || sym.section == EXTERN_S || sym.section == UND_S){
          int g = findGlobalSymbol(sym.name, globalSymbolTable, globalSymbolCount);
          if(g == -1) resolvable = 0;
          else target_addr = globalSymbolTable[g].value;
        } else {
          int sm = findSectionMap(i, sym.section, sectionMap, sectionMapSize);
          target_addr = sectionMap[sm].offset + sym.value;
        }
        if(resolvable){
          for(int k = 0; k < symTableSize; k++){
            if(strcmp(symTable[k].name, sym.name) == 0 &&
                symTableContent[k] == (uint32_t)target_addr){
              slot = symTable[k].slot;
              break;
            }
          }
        }

        if(slot == -1){
          int sm = findSectionMap(i, sym.section, sectionMap, sectionMapSize);
          int S  = sectionMap[sm].offset + sym.value;
          int D  = S - instruction_address;
          if(D > 0x7FF || D < -(0x7FF + 1)){
            fprintf(stderr, "LINKER ERROR: relocation overflow for symbol %s\n", sym.name);
            iF[i].error = LINK_ERROR_RELOCATION_OVERFLOW;
            continue;
          }
          int opcode = (*word >> 28) & 0xF;
          if(opcode == 0x3){
            uint32_t mod = (*word >> 24) & 0xF;
            mod &= 0x7;
            *word = (*word & 0xF0FFF000) | (mod << 24) | (D & 0xFFF);
          } else if(opcode == 0x2){
            *word = (*word & 0xF0FFF000) | (0x0 << 24) | (D & 0xFFF);
          } else {
            *word = (*word & 0xFFFFF000) | (D & 0xFFF);
          }
        } else {
          int slot_address = sym_table_start + slot * 4;
          int D = slot_address - instruction_address;
          if(D > 0x7FF || D < -(0x7FF + 1)){
            printf("LINKER ERROR: sym_table too far from instruction at 0x%X (symbol: %s, D=%d)\n",
              instruction_address, sym.name, D);
            iF[i].error = LINK_ERROR_RELOCATION_OVERFLOW;
            continue;
          }
          *word = (*word & 0xFFFFF000) | (D & 0xFFF);
        }
        iF[i].relocTable[r].type = RESOLVED;
      }
    }
  }
  return symTableSize;
};

void generateHex(Linker_init* lI, InputFile* iF, MergedSection* mergedSectionTable,
                 int mergedSectionTableSize, SectionMap* sectionMap, int sectionMapSize,
                 GlobalSymbol* globalSymbolTable, int globalSymbolCount){

#ifdef DEBUG_LINK
  for(int i = 0; i < mergedSectionTableSize; i++){
    fprintf(stderr, "GENHEX: '%s' start=0x%X size=%d placed=%d\n",
      mergedSectionTable[i].name, mergedSectionTable[i].start,
      mergedSectionTable[i].size, mergedSectionTable[i].placed);
  }
  for(int p = 0; p < lI->place_count; p++){
    fprintf(stderr, "GENHEX PLACE[%d]: '%s' @ 0x%lX\n",
      p, lI->place[p].section, (unsigned long)lI->place[p].address);
  }
#endif

  emit8(lI->output, 0x00535F454C463634);
  int header_offset_placeholder = 0;
  emit4(lI->output, header_offset_placeholder);
  emit4(lI->output, 0x00000000);

  Place sorted_places[lI->place_count];
  memcpy(sorted_places, lI->place, lI->place_count * sizeof(Place));
  for(int i = 1; i < lI->place_count; i++){
    Place key = sorted_places[i];
    int j = i - 1;
    while(j >= 0 && (unsigned long)sorted_places[j].address > (unsigned long)key.address){
      sorted_places[j+1] = sorted_places[j];
      j--;
    }
    sorted_places[j+1] = key;
  }

  typedef struct { unsigned int load_address; int file_offset; int size; } Segment;
  Segment segments[mergedSectionTableSize + 3];
  int segment_count = 0;
  int cursor = 0x10;

  int written[mergedSectionTableSize];
  memset(written, 0, sizeof(written));

  for(int p = 0; p < lI->place_count; p++){
    int placed_idx = findMergedSection(sorted_places[p].section, mergedSectionTable, mergedSectionTableSize);
    if(placed_idx == -1) continue;

    unsigned int seg_load_addr = (unsigned int)mergedSectionTable[placed_idx].start;
    int seg_file_off = cursor;
    int seg_size = 0;

    unsigned int next_place_start = 0xFFFFFFFFu;
    if(p + 1 < lI->place_count){
      int next_idx = findMergedSection(sorted_places[p+1].section, mergedSectionTable, mergedSectionTableSize);
      if(next_idx != -1)
        next_place_start = (unsigned int)mergedSectionTable[next_idx].start;
    }

    int order[mergedSectionTableSize];
    int order_count = 0;
    for(int i = 0; i < mergedSectionTableSize; i++){
      MergedSection* m = &mergedSectionTable[i];
      if(!m->placed || written[i]) continue;
      if(strcmp(m->name, "__sym_table__") == 0) continue;
      if((unsigned int)m->start >= seg_load_addr && (unsigned int)m->start < next_place_start)
        order[order_count++] = i;
    }
    for(int i = 1; i < order_count; i++){
      int key = order[i];
      int j = i - 1;
      while(j >= 0 && (unsigned int)mergedSectionTable[order[j]].start > (unsigned int)mergedSectionTable[key].start){
        order[j+1] = order[j];
        j--;
      }
      order[j+1] = key;
    }
    for(int k = 0; k < order_count; k++){
      MergedSection* m = &mergedSectionTable[order[k]];
      if(m->size > 0)
        fwrite(m->content, 1, m->size, lI->output);
      seg_size += m->size;
      cursor   += m->size;
      written[order[k]] = 1;
    }
    if(seg_size > 0)
      segments[segment_count++] = (Segment){seg_load_addr, seg_file_off, seg_size};
  }

  int leftover_load = 0, leftover_file = cursor, leftover_size = 0;
  for(int i = 0; i < mergedSectionTableSize; i++){
    if(written[i]) continue;
    MergedSection* m = &mergedSectionTable[i];
    if(strcmp(m->name, "__sym_table__") == 0) continue;
    if(!m->placed || m->size == 0){ written[i] = 1; continue; }
    if(leftover_size == 0) leftover_load = m->start;
    fwrite(m->content, 1, m->size, lI->output);
    leftover_size += m->size;
    cursor += m->size;
    written[i] = 1;
  }
  if(leftover_size > 0)
    segments[segment_count++] = (Segment){leftover_load, leftover_file, leftover_size};

  for(int i = 0; i < mergedSectionTableSize; i++){
    MergedSection* m = &mergedSectionTable[i];
    if(strcmp(m->name, "__sym_table__") != 0) continue;
    segments[segment_count++] = (Segment){m->start, cursor, m->size};
    fwrite(m->content, 1, m->size, lI->output);
    cursor += m->size;
    break;
  }

  int sym_seg_file_off = cursor;
  int sym_seg_size = 0;
  for(int i = 0; i < lI->input_count; i++){
    for(int s = 0; s < iF[i].symbolTable_count; s++){
      Symbol sym = iF[i].symbolTable[s];
      if(sym.type != STR && sym.type != DATA) continue;
      if(sym.section < 0) continue;
      int sm = findSectionMap(i, sym.section, sectionMap, sectionMapSize);
      if(sm == -1) continue;
      int final_address = sectionMap[sm].offset + sym.value;
      uint16_t name_len = (uint16_t)strlen(sym.name);
      emit4(lI->output, final_address);
      emit4(lI->output, sym.size);
      emit1(lI->output, sym.type);
      emit1(lI->output, sym.binding);
      emit2(lI->output, name_len);
      fwrite(sym.name, 1, name_len, lI->output);
      int entry_size = 4 + 4 + 1 + 1 + 2 + name_len;
      sym_seg_size += entry_size;
      cursor += entry_size;
    }
  }
  if(sym_seg_size > 0)
    segments[segment_count++] = (Segment){0xFFFFFFFF, sym_seg_file_off, sym_seg_size};

  int headerOffset = cursor;
  emit4(lI->output, segment_count);
  for(int i = 0; i < segment_count; i++){
    emit4(lI->output, segments[i].load_address);
    emit4(lI->output, segments[i].size);
    emit4(lI->output, segments[i].file_offset);
  }

  fseek(lI->output, 0x08, SEEK_SET);
  fwrite(&headerOffset, 4, 1, lI->output);
};

void generateRelocatable(Linker_init* lI, InputFile* iF, MergedSection* mergedSectionTable, int mergedSectionTableSize){
  int realSectionCount = 0;
  for(int i = 0; i < mergedSectionTableSize; i++){
    if(mergedSectionTable[i].name == NULL) break;
    if(strcmp(mergedSectionTable[i].name, "__sym_table__") == 0) continue;
    realSectionCount++;
  }

  int totalInputSyms = 0;
  for(int i = 0; i < lI->input_count; i++)
    totalInputSyms += iF[i].symbolTable_count;

  Symbol* newSyms = malloc((realSectionCount + totalInputSyms) * sizeof(Symbol));
  int newSymCount = 0;

  int** remap = malloc(lI->input_count * sizeof(int*));
  for(int i = 0; i < lI->input_count; i++){
    remap[i] = malloc(iF[i].symbolTable_count * sizeof(int));
    for(int s = 0; s < iF[i].symbolTable_count; s++) remap[i][s] = -1;
  }

  int* sectSymIndexForMerged = malloc(realSectionCount * sizeof(int));

  int mergedIdx = 0;
  for(int i = 0; i < mergedSectionTableSize; i++){
    if(mergedSectionTable[i].name == NULL) break;
    if(strcmp(mergedSectionTable[i].name, "__sym_table__") == 0) continue;
    Symbol sect;
    sect.name    = strdup(mergedSectionTable[i].name);
    sect.value   = 0;
    sect.size    = 0;
    sect.type    = SECT;
    sect.binding = LOCAL;
    sect.section = mergedIdx;
    sect.fd      = NULL;
    sectSymIndexForMerged[mergedIdx] = newSymCount;
    newSyms[newSymCount++] = sect;
    mergedIdx++;
  }
  for(int i = 0; i < lI->input_count; i++){
    for(int s = 0; s < iF[i].symbolTable_count; s++){
      Symbol sym = iF[i].symbolTable[s];
      if(sym.type == SECT){
        int mi = findMergedSection(sym.name, mergedSectionTable, mergedSectionTableSize);
        if(mi != -1){
          int realIdx = 0;
          for(int k = 0; k < mi; k++){
            if(strcmp(mergedSectionTable[k].name, "__sym_table__") != 0)
              realIdx++;
          }
          remap[i][s] = sectSymIndexForMerged[realIdx];
        }
        continue;
      }
      if(sym.binding == GLOBAL || sym.binding == EXTERN ||
         sym.section == UND_S || sym.section == EXTERN_S){
        int existing = -1;
        for(int k = 0; k < newSymCount; k++){
          if(newSyms[k].type == SECT) continue;
          if(strcmp(newSyms[k].name, sym.name) == 0){ existing = k; break; }
        }
        if(existing != -1){
          int existUndef = (newSyms[existing].section == UND_S ||
                            newSyms[existing].section == EXTERN_S);
          int thisDefined = (sym.section >= 0 || sym.section == ABS_S);
          if(existUndef && thisDefined){
            if(sym.section == ABS_S){
              newSyms[existing].value   = sym.value;
              newSyms[existing].section = ABS_S;
            } else {
              int mi = findMergedSection(iF[i].sectionTable[sym.section].name, mergedSectionTable, mergedSectionTableSize);
              int realIdx = 0;
              for(int k = 0; k < mi; k++)
                if(strcmp(mergedSectionTable[k].name, "__sym_table__") != 0) realIdx++;
              int f_off = 0;
              MergedSection* m = &mergedSectionTable[mi];
              for(int k = 0; k < m->file_count; k++){
                if(m->filesArray[k] == i && m->sectionArray[k] == sym.section){
                  f_off = (int)m->offsetArray[k]; break;
                }
              }
              newSyms[existing].value   = f_off + sym.value;
              newSyms[existing].section = realIdx;
            }
            newSyms[existing].type    = sym.type;
            newSyms[existing].binding = GLOBAL;
          }
          remap[i][s] = existing;
          continue;
        }
        Symbol ns = sym;
        ns.name = strdup(sym.name);
        ns.fd   = NULL;
        if(sym.section == UND_S || sym.section == EXTERN_S){
          ns.value   = 0;
          ns.section = sym.section;
          ns.binding = (sym.binding == EXTERN) ? EXTERN : GLOBAL;
        } else if(sym.section == ABS_S){
          ns.value   = sym.value;
          ns.section = ABS_S;
        } else {
          int mi = findMergedSection(iF[i].sectionTable[sym.section].name,
                                     mergedSectionTable, mergedSectionTableSize);
          int realIdx = 0;
          for(int k = 0; k < mi; k++)
            if(strcmp(mergedSectionTable[k].name, "__sym_table__") != 0) realIdx++;
          int f_off = 0;
          MergedSection* m = &mergedSectionTable[mi];
          for(int k = 0; k < m->file_count; k++){
            if(m->filesArray[k] == i && m->sectionArray[k] == sym.section){
              f_off = (int)m->offsetArray[k]; break;
            }
          }
          ns.value   = f_off + sym.value;
          ns.section = realIdx;
        }
        remap[i][s] = newSymCount;
        newSyms[newSymCount++] = ns;
        continue;
      }
      
      Symbol ns = sym;
      ns.name = strdup(sym.name);
      ns.fd   = NULL;
      if(sym.section == ABS_S){
        ns.value   = sym.value;
        ns.section = ABS_S;
      } else if(sym.section == UND_S || sym.section == EXTERN_S){
        ns.value   = 0;
        ns.section = sym.section;
      } else {
        int mi = findMergedSection(iF[i].sectionTable[sym.section].name, mergedSectionTable, mergedSectionTableSize);
        int realIdx = 0;
        for(int k = 0; k < mi; k++)
          if(strcmp(mergedSectionTable[k].name, "__sym_table__") != 0) realIdx++;
        int f_off = 0;
        MergedSection* m = &mergedSectionTable[mi];
        for(int k = 0; k < m->file_count; k++){
          if(m->filesArray[k] == i && m->sectionArray[k] == sym.section){
            f_off = (int)m->offsetArray[k]; break;
          }
        }
        ns.value = f_off + sym.value;
        ns.section = realIdx;
      }
      remap[i][s] = newSymCount;
      newSyms[newSymCount++] = ns;
    }
  }


  int totalRelocs = 0;
  for(int i = 0; i < lI->input_count; i++)
    totalRelocs += iF[i].relocTable_count;

  Relocation* newRel = malloc((totalRelocs + 1) * sizeof(Relocation));
  int newRelCount = 0;

  for(int i = 0; i < lI->input_count; i++){
    for(int r = 0; r < iF[i].relocTable_count; r++){
      Relocation rel = iF[i].relocTable[r];
      if(rel.type == RESOLVED)      continue;
      if(rel.type == UNINITIALISED) continue;
      

      int mi = findMergedSection(iF[i].sectionTable[rel.section].name, mergedSectionTable, mergedSectionTableSize);
      if(mi == -1) continue;
      int realIdx = 0;
      for(int k = 0; k < mi; k++)
        if(strcmp(mergedSectionTable[k].name, "__sym_table__") != 0) realIdx++;

      int f_off = 0;
      MergedSection* m = &mergedSectionTable[mi];
      for(int k = 0; k < m->file_count; k++){
        if(m->filesArray[k] == i && m->sectionArray[k] == rel.section){
          f_off = (int)m->offsetArray[k]; break;
        }
      }

      Relocation nr;
      nr.section    = realIdx;
      nr.symbol     = remap[i][rel.symbol];
      nr.offset     = f_off + rel.offset;
      nr.addend     = rel.addend;
      nr.type       = rel.type;
      nr.multiplier = 1;
      newRel[newRelCount++] = nr;
    }
  }

  
  emit8(lI->output, 0x00535F454C463634);
  emit8(lI->output, 0x0000000000000000);


  int* sectionContentLen = malloc(realSectionCount * sizeof(int));
  {
    int realIdx = 0;
    for(int i = 0; i < mergedSectionTableSize; i++){
      if(mergedSectionTable[i].name == NULL) break;
      if(strcmp(mergedSectionTable[i].name, "__sym_table__") == 0) continue;
      MergedSection* m = &mergedSectionTable[i];
      if(m->size > 0)
        fwrite(m->content, 1, m->size, lI->output);
      sectionContentLen[realIdx] = m->size;
      realIdx++;
    }
  }

  
  int* symNameOffset = malloc(newSymCount * sizeof(int));
  int symPoolSize = 0;
  for(int k = 0; k < newSymCount; k++){
    symNameOffset[k] = symPoolSize;
    int len = strlen(newSyms[k].name);
    fwrite(newSyms[k].name, 1, len, lI->output);
    emit1(lI->output, 0);
    symPoolSize += len + 1;
  }

  
  int symTableSize = 0;
  for(int k = 0; k < newSymCount; k++){
    emit4(lI->output, symNameOffset[k]);
    emit4(lI->output, newSyms[k].value);
    emit4(lI->output, newSyms[k].size);
    emit1(lI->output, (uint8_t)newSyms[k].type);
    emit1(lI->output, (uint8_t)newSyms[k].binding);
    emit2(lI->output, (uint16_t)(short)newSyms[k].section);
    symTableSize += 16;
  }

  
  int relTableSize = 0;
  for(int k = 0; k < newRelCount; k++){
    emit4(lI->output, newRel[k].section);
    emit4(lI->output, newRel[k].symbol);
    emit4(lI->output, newRel[k].offset);
    emit4(lI->output, newRel[k].addend);
    emit1(lI->output, (uint8_t)newRel[k].type);
    emit3(lI->output, 0);
    relTableSize += 20;
  }

  
  int headerEntryCount = realSectionCount + 4;
  char** secNames = malloc(headerEntryCount * sizeof(char*));
  int* secLens  = malloc(headerEntryCount * sizeof(int));
  {
    int realIdx = 0;
    for(int i = 0; i < mergedSectionTableSize; i++){
      if(mergedSectionTable[i].name == NULL) break;
      if(strcmp(mergedSectionTable[i].name, "__sym_table__") == 0) continue;
      secNames[realIdx] = mergedSectionTable[i].name;
      secLens[realIdx]  = sectionContentLen[realIdx];
      realIdx++;
    }
    secNames[realSectionCount + 0] = "Sym_pool";   secLens[realSectionCount + 0] = symPoolSize;
    secNames[realSectionCount + 1] = "Sym_table";  secLens[realSectionCount + 1] = symTableSize;
    secNames[realSectionCount + 2] = "Rel_Table";  secLens[realSectionCount + 2] = relTableSize;
    secNames[realSectionCount + 3] = "Sect_Pool";  secLens[realSectionCount + 3] = 0;
  }

  int* secNameOffset = malloc(headerEntryCount * sizeof(int));
  int sectPoolSize = 0;
  for(int k = 0; k < headerEntryCount; k++){
    secNameOffset[k] = sectPoolSize;
    int len = strlen(secNames[k]);
    fwrite(secNames[k], 1, len, lI->output);
    emit1(lI->output, 0);
    sectPoolSize += len + 1;
  }

  
  long headerOffset = ftell(lI->output);
  emit4(lI->output, sectPoolSize);

  
  int base = 0x10;
  for(int k = 0; k < headerEntryCount; k++){
    emit4(lI->output, secNameOffset[k]);
    emit4(lI->output, secLens[k]);
    emit4(lI->output, base);
    base += secLens[k];
  }
  
  fseek(lI->output, 0x0C, SEEK_SET);
  int ho = (int)headerOffset;
  fwrite(&ho, 4, 1, lI->output);
  
  for(int i = 0; i < lI->input_count; i++) free(remap[i]);
  free(remap);
  free(newSyms);
  free(newRel);
  free(sectSymIndexForMerged);
  free(symNameOffset);
  free(sectionContentLen);
  free(secNames);
  free(secLens);
  free(secNameOffset);
};