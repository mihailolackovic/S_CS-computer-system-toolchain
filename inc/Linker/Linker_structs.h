#ifndef LINKER_STRUCTS_H
#define LINKER_STRUCTS_H

#include "Command_line_args.h"
#include "../common/ObjectFile_elements.h"

typedef struct {
  char* file_name;

  Symbol* symbolTable;
  
  Section* sectionTable;

  Relocation* relocTable;

  short sectionTable_count;
  short symbolTable_count;
  short relocTable_count;
  
  LinkERROR error;
} InputFile;

typedef struct{
  char* name;
  int start;
  int size;
  int* filesArray;
  int* sectionArray;
  unsigned long* offsetArray;
  int file_count;
  int placed;
  unsigned char* content;
}MergedSection;

int findMergedSection(char* name, MergedSection* mergedSectionTable, int size);

typedef struct{
  int file;
  int section;

  int merged;
  unsigned long offset;
} SectionMap;

int addSectionMap(int file, int section, int merged, int offset, SectionMap* sectionMap);
int findSectionMap(int file, int section, SectionMap* sectionMap, int size);

typedef struct{
  char* name;
  unsigned long value;
} GlobalSymbol;

int findGlobalSymbol(char* name, GlobalSymbol* table, int size);

typedef struct {
  char* name;
  int slot;
} SymTableEntry;



#endif