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

AsmERROR labelSymbol(char* label, Assembler_state* a){
  int sectionIndex = findSection(a->current_section, a->sectionTable);
  if(sectionIndex < 0)
    return SYMBOL_NOT_IN_SECTION;

  int symbolIndex = findSymbol(label, a->sectionTable);

  if(symbolIndex == -1){
    Symbol sym = (Symbol){label, a->LC, 0, NOTYPE, LOCAL, sectionIndex};
    addSymbol(sym , a->symbolTable);
  }
  else{
    if(a->symbolTable[symbolIndex].section >= 0)
      return DOUBLE_SYMBOL;
    a->symbolTable[symbolIndex].value = a->LC;
    a->symbolTable[symbolIndex].section = a->current_section;
  }
  return PASS;
};

AsmERROR execute_function(Functions function, Operand* operand_list, int operand_count, Assembler_state* a){
  int index;
  uint8_t byte = 0x00;
  switch(function)  {
  case OPCODE_SHR:
    byte = 0x01;
  case OPCODE_SHL:
    if(operand_count != 2)  return INVALID_OPERAND;
    if(operand_list[0].type != OPERAND_TYPE_REGISTER || operand_list[0].dereferance_flag != 0) return INVALID_OPERAND;
    if(operand_list[1].type != OPERAND_TYPE_REGISTER || operand_list[0].dereferance_flag != 0) return INVALID_OPERAND;
    emit4(a->output, 0x70000000+(byte << (6*4))+
      (registerIndex(operand_list[0].reg) << (5*4)) + (registerIndex(operand_list[0].reg) << (4*4))
      + (registerIndex(operand_list[1].reg) << (3*4)));
    a->LC += 4;
    return PASS;

  case OPCODE_AND:
    byte = 0x01;
  case OPCODE_OR:
    if(byte == 0x00)  byte = 0x02;
  case OPCODE_XOR:
    if(byte == 0x00)  byte = 0x03;
    if(operand_count != 2)  return INVALID_OPERAND;
    if(operand_list[0].type != OPERAND_TYPE_REGISTER || operand_list[0].dereferance_flag != 0) return INVALID_OPERAND;
    if(operand_list[1].type != OPERAND_TYPE_REGISTER || operand_list[0].dereferance_flag != 0) return INVALID_OPERAND;
    emit4(a->output, 0x60000000+(byte << (6*4))+
      (registerIndex(operand_list[0].reg) << (5*4)) + (registerIndex(operand_list[0].reg) << (4*4))
      + (registerIndex(operand_list[1].reg) << (3*4)));
    a->LC += 4;
    return PASS;

  case OPCODE_NOT:
    if(operand_count != 1)  return INVALID_OPERAND;
    if(operand_list[0].type != OPERAND_TYPE_REGISTER || operand_list[0].dereferance_flag != 0) return INVALID_OPERAND;
    emit4(a->output, 0x60000000+
      (registerIndex(operand_list[0].reg) << (5*4)) + (registerIndex(operand_list[0].reg) << (4*4)));
    a->LC += 4;
    return PASS;

  case OPCODE_BGT:
    if(byte == 0x00)  byte = 0x03;
  case OPCODE_BNE:
    if(byte == 0x00)  byte = 0x02;
  case OPCODE_BEQ:
    if(byte == 0x00)  byte = 0x01;
  case OPCODE_JMP:
    if(operand_list->type==OPERAND_TYPE_EXPRESSION) byte += 8;
    if(operand_count != 1)  return INVALID_OPERAND;


  case OPCODE_SUB:
    byte = 0x01;
  case OPCODE_MUL:
    if(byte == 0x00)  byte = 0x02;
  case OPCODE_DIV:
    if(byte == 0x00)  byte = 0x03;
  case OPCODE_ADD:
    if(operand_count != 2)  return INVALID_OPERAND;
    if(operand_list[0].type != OPERAND_TYPE_REGISTER || operand_list[0].dereferance_flag != 0) return INVALID_OPERAND;
    if(operand_list[1].type != OPERAND_TYPE_REGISTER || operand_list[0].dereferance_flag != 0) return INVALID_OPERAND;
    emit4(a->output, 0x50000000+(byte << (6*4))+
      (registerIndex(operand_list[0].reg) << (5*4)) + (registerIndex(operand_list[0].reg) << (4*4))
      + (registerIndex(operand_list[1].reg) << (3*4)));
    a->LC += 4;
    return PASS;

  case OPCODE_RET:
    if(byte == 0x00)  byte = 0x40;
  case OPCODE_IRET:
    if(byte == 0x00)  byte = 0x20;
  case OPCODE_HALT:
    if(operand_count != 0)  return INVALID_OPERAND;
    emit4(a->output, byte << (3*8));
    a->LC += 4;
    return PASS;

  case DIRECTIVE_SECTION:
    if(operand_count != 1) return INVALID_OPERAND;
    if(operand_list[0].type != OPERAND_TYPE_SYMBOL) return INVALID_OPERAND;
    Section section = {operand_list[0].symbol, 0, 0};
    index = findSection(a->current_section, a->sectionTable);
    if(index != -1)
      a->sectionTable[index].lenght = a->LC;
    a->LC = 0;
    index = addSection(section, a->sectionTable);
    if(index == -1) return DOUBLE_SECTION; 
    a->current_section = a->sectionTable[index].name;
    Symbol sym = {section.name, 0, 0, SECT, LOCAL, index};
    addSymbol(sym, a->symbolTable);
    return PASS;

  case DIRECTIVE_GLOBAL:
    for(int i = 0; i < operand_count; i++){
      if(operand_list[i].type != OPERAND_TYPE_SYMBOL) return INVALID_OPERAND;
      Symbol sym = {operand_list[i].symbol, 0, 0, NOTYPE, GLOBAL, findSection(a->current_section, a->sectionTable)};
      index = findSymbol(sym.name, a->symbolTable);
      if(index == -1) addSymbol(sym, a->symbolTable);
      else  a->symbolTable[index].binding = GLOBAL;
    }
    return PASS;

  case DIRECTIVE_EXTERN:
    for(int i = 0; i < operand_count; i++){
      if(operand_list[i].type != OPERAND_TYPE_SYMBOL) return INVALID_OPERAND;
      Symbol sym = {operand_list[i].symbol, 0, 0, NOTYPE, GLOBAL, -2};
      index = findSymbol(sym.name, a->symbolTable);
      if(index == -1) addSymbol(sym, a->symbolTable);
      else 
        return EXTERN_SYMBOL_REDEFINITION;
    }
    return PASS;

  case DIRECTIVE_END:
     if(operand_count != 0) return INVALID_OPERAND;
    a->assembler_flag = 0;
    index = findSection(a->current_section, a->sectionTable);
    if(index != -1)
      a->sectionTable[index].lenght = a->LC;
    return PASS;
  default:
  break;}
};