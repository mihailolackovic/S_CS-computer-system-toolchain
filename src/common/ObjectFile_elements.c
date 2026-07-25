#include "../../inc/common/ObjectFile_elements.h"

int addSection_O(Section section, Section* sectionTable, int sectionTable_lenght){
  for(int i = 0; i < sectionTable_lenght; i++){
    if(sectionTable[i].name){
      if(strcmp(section.name,sectionTable[i].name) == 0) return -1;
    }else  {
      sectionTable[i] = section;
      return i;
    }
  }
};
int findSection_O(char* section, Section* sectionTable, int sectionTable_lenght){
  for(int i = 0; i < sectionTable_lenght; i++){
    if(sectionTable[i].name){
      if(strcmp(section, sectionTable[i].name) == 0) 
        return i;
    } else
      break;
  }
  return -1;
};

int addSymbol_O(Symbol symbol, Symbol* symbolTable, int symbolTable_lenght){
  for(int i = 0; i < symbolTable_lenght; i++){
    if(symbolTable[i].name){
      if(strcmp(symbol.name, symbolTable[i].name) == 0) return 3;
      if(symbol.binding == EXTERN) return 5;
    }else  {
      symbolTable[i] = symbol;
      return 0;
    }
  }
  return 1;
};

int findSymbol_O(char* symbol, Symbol* symbolTable, int symbolTable_lenght){
  for(int i = 0; i < symbolTable_lenght; i++){
    if(symbolTable[i].name){
      if(strcmp(symbol, symbolTable[i].name) == 0) 
        return i;
    }else
      break;
  }
  return -1;
};

int addRelocation_O(Relocation relocation, Relocation* relocationTable, int relocationTable_lenght){
  for(int i = 0; i < relocationTable_lenght; i++){
    if(!relocationTable[i].type){
      relocationTable[i] = relocation;
      return 0;
    }
  }
  return 1;
};

SymbolType tot(char* type){
  if(strcmp(type, "FUNC") == 0)
    return FUNC;
  if(strcmp(type, "STR") == 0)
    return STR;
  if(strcmp(type, "DATA") == 0)
    return DATA;
  if(strcmp(type, "SECT") == 0)
    return SECT;
  if(strcmp(type, "SRC") == 0)
    return SRC;

  return NOTYPE;
};