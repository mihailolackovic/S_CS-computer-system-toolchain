#include "../../inc/Linker/Linker.h"

void Linker(Linker_init* lI){
  InputFile* iF = malloc(lI->input_count * sizeof(InputFile));
  extractInformation(lI, iF);

  MergedSection mergedSectionTable[lI->input_count*32];
  for(int i = 0; i < lI->input_count*32; i++){
    mergedSectionTable[i] = (MergedSection){NULL,0,0,NULL,NULL,NULL,0,0,NULL};
  }

  int mergedSectionTableSize = createMergedSectionTable(lI, iF, mergedSectionTable);

  SectionMap* sectionMap = malloc(mergedSectionTableSize*lI->input_count*sizeof(SectionMap));
  for(int k = 0; k < mergedSectionTableSize*lI->input_count; k++)
    sectionMap[k].file = -1;

  int sectionMapSize = createSectionMap(lI, iF, mergedSectionTable, sectionMap, mergedSectionTableSize);

  if(sectionMapSize == -1)
    return;

  int symbolCount = 0;
  for(int i = 0; i < lI->input_count; i++)
    symbolCount += iF[i].symbolTable_count;

  if(lI->arg == LINKER_ARG_RELOCATABLE) {
    generateRelocatable(lI, iF, mergedSectionTable, mergedSectionTableSize);
    return;
  }

  GlobalSymbol* globalSymbolTable = malloc((symbolCount+5)*sizeof(GlobalSymbol));
  int globalSymbolTableSize = createGlobalSymbolTable(lI, iF, sectionMap, sectionMapSize, globalSymbolTable);

  for(int i = 0; i < lI->input_count; i++){
    if(iF[i].error == LINK_ERROR_MULTIPLE_DEFINITION){
      printf("\n\nLINKER ERROR: multiple definitions for global symbol: %s, in file %s\n\n", iF[i].symbolTable[iF[i].symbolTable_count].name, iF[i].file_name);
      return;
    }
  }


  resolveRelocations(lI, iF, mergedSectionTable, mergedSectionTableSize, sectionMap,
                     sectionMapSize, globalSymbolTable, globalSymbolTableSize);

  mergedSectionTableSize++;

  for(int i = 0; i < lI->input_count; i++){
    if(iF[i].error != LINKER_ARG_NONE){
      printf("\n\nLINKER ERROR: relocation failed in %s\n\n", iF[i].file_name);
      return;
    }
  }

  generateHex(lI, iF, mergedSectionTable, mergedSectionTableSize, sectionMap,sectionMapSize, globalSymbolTable, globalSymbolTableSize);

  ouputText(lI);

  return;
};