#include "../../inc/Linker/Linker_structs.h"

int findMergedSection(char* name, MergedSection* mergedSectionTable, int size){
  for(int i = 0; i < size; i++){
    if(strcmp(name, mergedSectionTable[i].name)==0)
      return i;
  }
  return -1;
};


int addSectionMap(int file, int section, int merged, int offset, SectionMap* sectionMap){
  int i = 0;
  while(sectionMap[i].file != -1)
    i++;

  sectionMap[i] = (SectionMap){file, section, merged, offset};
  return i;
};
int findSectionMap(int file, int section, SectionMap* sectionMap, int size){
  for(int i = 0; i < size; i++){
    if(sectionMap[i].file == file && sectionMap[i].section == section)
      return i;
  }
  return -1;
}


int findGlobalSymbol(char* name, GlobalSymbol* table, int size){
  for(int i = 0; i < size; i++){
    if(strcmp(name, table[i].name) == 0)
      return i;
  }
  return -1;
}