#include "../../inc/Assembler/Assembler_structs.h"

AsmERROR addSection(Section section, Section* sectionTable){
  for(int i = 0; i < 10; i++){
    if(sectionTable[i].name){
      if(strcmp(section.name,sectionTable[i].name) == 0) return DOUBLE_SECTION;
    }else  {
      sectionTable[i] = section;
      return PASS;
    }
  }
};
int findSection(char* section, Section* sectionTable){
  for(int i = 0; i < 10; i++){
    if(sectionTable[i].name)
      if(strcmp(section, sectionTable[i].name) == 0) return i;
    else
      break;
  }
  return -1;
};

AsmERROR addSymbol(Symbol symbol, Symbol* symbolTable){
  for(int i = 0; i < 30; i++){
    if(symbolTable[i].name){
      if(strcmp(symbol.name, symbolTable[i].name) == 0) return DOUBLE_SYMBOL;
    }else  {
      symbolTable[i] = symbol;
      return PASS;
    }
  }
};

int findSymbol(char* symbol, Symbol* symbolTable){
  for(int i = 0; i < 30; i++){
    if(symbolTable[i].name)
      if(strcmp(symbol, symbolTable[i].name) == 0) return i;
    else
      break;
  }
  return -1;
};