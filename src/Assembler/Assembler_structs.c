#include "../../inc/Assembler/Assembler_structs.h"

int addSection(Section section, Section* sectionTable){
  for(int i = 0; i < SECTION_TABLE_LENGHT; i++){
    if(sectionTable[i].name){
      if(strcmp(section.name,sectionTable[i].name) == 0) return -1;
    }else  {
      sectionTable[i] = section;
      return i;
    }
  }
};
int findSection(char* section, Section* sectionTable){
  for(int i = 0; i < SECTION_TABLE_LENGHT; i++){
    if(sectionTable[i].name){
      if(strcmp(section, sectionTable[i].name) == 0) 
        return i;
    } else
      break;
  }
  return -1;
};

AsmERROR addSymbol(Symbol symbol, Symbol* symbolTable){
  for(int i = 0; i < SYMBOL_TABLE_LENGHT; i++){
    if(symbolTable[i].name){
      if(strcmp(symbol.name, symbolTable[i].name) == 0) return DOUBLE_SYMBOL;
      if(symbol.type == EXTERN) return EXTERN_SYMBOL_REDEFINITION;
    }else  {
      symbolTable[i] = symbol;
      return PASS;
    }
  }
  return TABLE_FULL;
};

int findSymbol(char* symbol, Symbol* symbolTable){
  for(int i = 0; i < SYMBOL_TABLE_LENGHT; i++){
    if(symbolTable[i].name){
      if(strcmp(symbol, symbolTable[i].name) == 0) 
        return i;
    }else
      break;
  }
  return -1;
};

int registerIndex(char* reg){
  switch (reg[0])
  {
  case 'r':
    return atoi(reg+1);
  case 's':
    if(reg[1] == 'p') return 14;
    return 0;
  case 'h':
    return 1;
  case 'c':
    return 2;
  case 'p':
    return 15;
  default:
    return -1;
  }
};


AsmERROR addRelocation(Relocation relocation, Relocation* relocationTable){
  for(int i = 0; i < RELOC_TABLE_LENGHT; i++){
    if(!relocationTable[i].type){
      relocationTable[i] = relocation;
      return PASS;
    }
  }
  return TABLE_FULL;
};