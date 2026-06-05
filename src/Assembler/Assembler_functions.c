#include "../../inc/Assembler/Assembler_functions.h"

Functions lookup_function(char* function){
  FunctionTableElement* table;
  size_t table_size;

  if(function[0] == '.'){
      table = directiveTable;
      table_size = sizeof(directiveTable) / sizeof(FunctionTableElement);
  }else  {
      table = opcodeTable;
      table_size = sizeof(opcodeTable) / sizeof(FunctionTableElement);
  }

  for(size_t i = 0; i < table_size; i++){
      if(strcmp(table[i].name, function) == 0)
          return table[i].dir;
  }
};

AsmERROR execute_function(Functions function, Operand* operand_list, int operand_count, Assembler_state* a){
  int index;
  switch(function)  {
  case OPCODE_HALT:
    if(operand_count != 0)  return INVALID_OPERAND;
    emit1(a->output, 0x00);
    a->LC++;
    return PASS;
  case DIRECTIVE_SECTION:
    if(operand_count != 1) return INVALID_OPERAND;
    if(operand_list[0].type != OPERAND_TYPE_SYMBOL) return INVALID_OPERAND;
    Section section = {operand_list[0].symbol, 0, 0};
    index = findSection(a->current_section, a->sectionTable);
    if(index != -1)
      a->sectionTable[index].lenght = a->LC;
    a->LC = 0;
    a->current_section = section.name;
    Symbol sym = {section.name, 0, 0, SECT, LOCAL, &section};
    addSymbol(sym, a->symbolTable);
    return addSection(section, a->sectionTable);
  case DIRECTIVE_GLOBAL:
    for(int i = 0; i < operand_count; i++){
      if(operand_list[i].type != OPERAND_TYPE_SYMBOL) return INVALID_OPERAND;
      Symbol sym = {operand_list[i].symbol, 0, 0, NOTYPE, GLOBAL, a->current_section};
      index = findSymbol(sym.name, a->symbolTable);
      if(index == -1) addSymbol(sym, a->symbolTable);
      else  a->symbolTable[index].binding = GLOBAL;
    }
    return PASS;
  default:
  break;}
};