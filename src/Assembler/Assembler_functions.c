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
  switch(function)  {
  case OPCODE_HALT:
    if(operand_count != 0)  return INVALID_OPERAND;
    emit1(a->output, 0x00);
    a->LC++;
    return PASS;
  case DIRECTIVE_GLOBAL:
    for(int i = 0; i < operand_count; i++){
      if(operand_list[i].type != OPERAND_TYPE_SYMBOL) return INVALID_OPERAND;
      Symbol sym = {operand_list[i].symbol, 0, 0, DATA, GLOBAL, a->current_section};
      if(addSymbol(sym, a->symbolTable) != PASS)  return DOUBLE_SYMBOL;
      return PASS;
    }
  default:
  break;}
};