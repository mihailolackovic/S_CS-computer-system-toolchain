#include "../../inc/Assembler/Assembler_structs.h"

AsmERROR addSymbol(Symbol symbol, Symbol* symbolTable){
  for(int i = 0; i < 20; i++){
    if(symbolTable[i].name){
      if(strcmp(symbol.name,symbolTable[i].name) == 0) return DOUBLE_SYMBOL; // SYMBOL IN TABLE HELP
    }else  {
      symbolTable[i] = symbol;
      return PASS;
    }
  }
};