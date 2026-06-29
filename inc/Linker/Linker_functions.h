#ifndef LINKER_FUNCTIONS_H
#define LINKER_FUNCTIONS_H

#include "../common/emit.h"
#include "Linker_structs.h"
#include <stdint.h>

void extractInformation(Linker_init* lI, InputFile* iF);

int createMergedSectionTable(Linker_init* lI, InputFile* iF, MergedSection* mergedSectionTable);

int createSectionMap(Linker_init* lI, InputFile* iF, MergedSection* mergedSectionTable, SectionMap* sectionMap, int sectionTableSize);

int createGlobalSymbolTable(Linker_init* lI, InputFile* iF, SectionMap* sectionMap, int sectionTableSize, GlobalSymbol* globalSymbolTable);

int resolveRelocations(
  Linker_init* lI,
  InputFile* iF,
  MergedSection* mergedSectionTable,
  int mergedSectionTableSize,
  SectionMap* sectionMap,
  int sectionMapSize,
  GlobalSymbol* globalSymbolTable,
  int globalSymbolCount);

void generateHex(Linker_init* lI, InputFile* iF, MergedSection* mergedSectionTable, 
  int mergedSectionTableSize, SectionMap* sectionMap, int sectionMapSize,
  GlobalSymbol* globalSymbolTable, int globalSymbolCount);


  
#endif