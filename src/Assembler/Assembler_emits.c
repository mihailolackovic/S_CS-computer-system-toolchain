#include "../../inc/Assembler/Assembler_emits.h"

void emitSymsTable(Assembler_state* a){
  int Sym_Pool_size = 0;
  int Sym_Table_size = 0;
  int sym_name_offset[SYMBOL_TABLE_LENGHT];
  for(int i = 0; i < SYMBOL_TABLE_LENGHT; i++){
    if(a->symbolTable[i].name){
      sym_name_offset[i] = Sym_Pool_size;
      Sym_Pool_size += strlen(a->symbolTable[i].name)+1;
      for(int j = 0; j < strlen(a->symbolTable[i].name); j++)
        emit1(a->output, a->symbolTable[i].name[j]);
      emit1(a->output, 0);
    } else break;
  }
  for(int i = 0; i < SYMBOL_TABLE_LENGHT; i++){
    if(a->symbolTable[i].name){
      Sym_Table_size += 16;
      emit4(a->output, sym_name_offset[i]);
      emit4(a->output, a->symbolTable[i].value);
      emit4(a->output, a->symbolTable[i].size);
      emit1(a->output, a->symbolTable[i].type);
      emit1(a->output, a->symbolTable[i].binding);
      emit2(a->output, a->symbolTable[i].section);
    } else break;
  }
  if(a->sectionTable[SECTION_TABLE_LENGHT-5].name != NULL)
    return;
  addSection((Section){"Sym_pool", 0, Sym_Pool_size}, a->sectionTable);
  addSection((Section){"Sym_table", 0, Sym_Table_size}, a->sectionTable);
};

void emitRelocTable(Assembler_state* a){
  int reloc_table_size = 0;
  for(int i = 0; i < RELOC_TABLE_LENGHT; i++){
    if(a->relocTable[i].type != UNINITIALISED){
      if(a->relocTable[i].type == RESOLVED) continue;
      emit4(a->output, a->relocTable[i].section);
      emit4(a->output, a->relocTable[i].symbol);
      emit4(a->output, a->relocTable[i].offset);
      emit4(a->output, a->relocTable[i].addend);
      emit1(a->output, a->relocTable[i].type);
      emit3(a->output, 0);
      reloc_table_size += 20;
    }else break;
  }
  addSection((Section){"Rel_Table", 0, reloc_table_size}, a->sectionTable);
};

AsmERROR emitSectionHeader(Assembler_state* a){
  int sec_Pool_size = 0;
  int sec_name_offset[SECTION_TABLE_LENGHT];
  int header_size = 0x10;
  int cursor = header_size;
  addSection((Section){"Sect_Pool", 0, 0}, a->sectionTable);
  for(int i = 0; i < SECTION_TABLE_LENGHT; i++){
    if(a->sectionTable[i].name != NULL){
      sec_name_offset[i] = sec_Pool_size;
      sec_Pool_size += strlen(a->sectionTable[i].name)+1;
      for(int j = 0; j < strlen(a->sectionTable[i].name); j++)
        emit1(a->output, a->sectionTable[i].name[j]);
      emit1(a->output, 0);
    } else break;
  }
  a->headerOffset = ftell(a->output);
  emit4(a->output, sec_Pool_size);
  for(int i = 0; i < SECTION_TABLE_LENGHT; i++){
    if(a->sectionTable[i].name != NULL){
      a->sectionTable[i].base = cursor;
      cursor += a->sectionTable[i].lenght;
      emit4(a->output ,sec_name_offset[i]);
      emit4(a->output ,a->sectionTable[i].lenght);
      emit4(a->output ,a->sectionTable[i].base);
    } else break;
  }
  return PASS;
};