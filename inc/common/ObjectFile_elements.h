#ifndef OBJECT_FILE_ELEMENTS_H
#define OBJECT_FILE_ELEMENTS_H
////////////////////////////////////////////////////////////
//  RELOC  /////////////////////////////////////////////////
////////////////////////////////////////////////////////////

typedef struct ForwardDeclaration ForwardDeclaration;

#define SYMBOL_TABLE_LENGHT 128
#define SECTION_TABLE_LENGHT 32
#define RELOC_TABLE_LENGHT 128

typedef enum{
  UND_S = -1,
  EXTERN_S = -2,
  ABS_S = SECTION_TABLE_LENGHT
} SpecialSections;

typedef enum{
  UNINITIALISED,
  R_ABS_12,
  R_PC_12,
  R_ABS_32,
  R_PC_32,
  RESOLVED
} RelocType;

typedef struct{
  int section;
  int symbol;
  int offset;
  int addend;
  RelocType type;
  int multiplier;
} Relocation;

int addRelocation_O(Relocation relocation, Relocation* relocationTable, int relocationTable_lenght);

////////////////////////////////////////////////////////////
//  SECTION  ///////////////////////////////////////////////
////////////////////////////////////////////////////////////

typedef struct{
  char* name;
  int base;
  int lenght;
} Section;

int addSection_O(Section section, Section* sectionTable, int sectionTable_lenght);
int findSection_O(char* section, Section* sectionTable, int sectionTable_lenght);

////////////////////////////////////////////////////////////
//  SYMBOLS  ///////////////////////////////////////////////
////////////////////////////////////////////////////////////

typedef enum{
  NOTYPE,
  DATA,
  STR,
  FUNC,
  SECT,
  SRC
} SymbolType;
typedef enum{
  LOCAL,
  GLOBAL,
  EXTERN
} SymbolBinding;

typedef struct{
  char* name;
  int value;
  int size;
  SymbolType type;
  SymbolBinding binding;
  int section;
  ForwardDeclaration* fd;
} Symbol;

int addSymbol_O(Symbol symbol, Symbol* symbolTable, int symbolTable_lenght);
int findSymbol_O(char* symbol, Symbol* symbolTable, int symbolTable_lenght);

////////////////////////////////////////////////////////////
#endif