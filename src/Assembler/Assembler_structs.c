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
    }else  {
      symbolTable[i] = symbol;
      return PASS;
    }
  }
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