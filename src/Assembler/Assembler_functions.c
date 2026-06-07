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
    Symbol sym = (Symbol){label, a->LC, 0, NOTYPE, LOCAL, sectionIndex, NULL};
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
  uint8_t OC = 0, M = 0, regA = 0, regB = 0, regC = 0;
  uint8_t byte = 0x00;
  uint64_t eightbyte = 0x00;
  switch(function)  {
  case OPCODE_INT:
    if(operand_count != 0) return INVALID_OPERAND;
    instructionEmit(a->output, (Ins_emit){0x1, 0, 0, 0, 0, 0});
    a->LC += 4;
    return PASS;

  case OPCODE_RET:
    if(operand_count != 0) return INVALID_OPERAND;
    instructionEmit(a->output, (Ins_emit){0x9, 3, 15, 14, 0, 0x4});
    a->LC += 4;
    return PASS;

  case OPCODE_IRET:
    if(operand_count != 0) return INVALID_OPERAND;
    instructionEmit(a->output, (Ins_emit){0x9, 3, 0, 14, 0, 0x4});
    instructionEmit(a->output, (Ins_emit){0x9, 3, 15, 14, 0, 0x4});
    a->LC += 8;
    return PASS;

  case OPCODE_CSRRD:
  case OPCODE_CSRWR:
  case OPCODE_XCHG:
    if(operand_count != 2) return INVALID_OPERAND;
    if(operand_list[0].type != OPERAND_TYPE_REGISTER) return INVALID_OPERAND;
    if(operand_list[1].type != OPERAND_TYPE_REGISTER) return INVALID_OPERAND;
    if(function == OPCODE_XCHG)
      instructionEmit(a->output, (Ins_emit){0x4, 0, 0 ,registerIndex(operand_list[1].reg), registerIndex(operand_list[0].reg), 0, 0});
    else if(function == OPCODE_CSRRD)
      instructionEmit(a->output, (Ins_emit){0x9, 0, registerIndex(operand_list[1].reg), registerIndex(operand_list[0].reg), 0, 0, 0});
    else if(function == OPCODE_CSRWR)
      instructionEmit(a->output, (Ins_emit){0x9, 4, registerIndex(operand_list[1].reg), registerIndex(operand_list[0].reg), 0, 0, 0});
    a->LC += 4;
    return PASS;

  case OPCODE_POP:
    if(operand_count != 1) return INVALID_OPERAND;
    if(operand_list[0].type != OPERAND_TYPE_REGISTER) return INVALID_OPERAND;
    instructionEmit(a->output, (Ins_emit){0x9, 3, registerIndex(operand_list[0].reg), 14, 0, 0x4});
    a->LC += 4;
    return PASS;
  case OPCODE_PUSH:
    if(operand_count != 1) return INVALID_OPERAND;
    if(operand_list[0].type != OPERAND_TYPE_REGISTER) return INVALID_OPERAND;
    instructionEmit(a->output, (Ins_emit){0x8, 1, 14, 0, registerIndex(operand_list[0].reg), 0xFFFC});
    a->LC += 4;
    return PASS;
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
    if(M == 0x00)  M = 0x03;
  case OPCODE_BNE:
    if(M == 0x00)  M = 0x02;
  case OPCODE_BEQ:
    if(M == 0x00)  M = 0x01;
    if(operand_count != 3)  return INVALID_OPERAND;
    if(operand_list[2].dereferance_flag != 1) return INVALID_OPERAND;
    OC = 0x3;
    regA = 0;
    regB = registerIndex(operand_list[0].reg);
    regC = registerIndex(operand_list[1].reg);
  case OPCODE_JMP:
    if(function == OPCODE_JMP) {
      OC = 0x3;
      regA = 0;
      regB = 0;
      regC = 0;
      if(operand_count != 1)  return INVALID_OPERAND;
    }
    if(operand_list[(function==OPCODE_JMP)?0:2].type==OPERAND_TYPE_NUMBER){
      if(operand_list[(function==OPCODE_JMP)?0:2].number > 0x7FF ||
         operand_list[(function==OPCODE_JMP)?0:2].number < -(0x7FF + 1))
        return JUMP_OVERFLOW;
      instructionEmit(a->output, (Ins_emit){OC,M,regA,regB,regC,operand_list[(function==OPCODE_JMP)?0:2].number} );
    }
    if(operand_list[(function==OPCODE_JMP)?0:2].type==OPERAND_TYPE_SYMBOL){
      index = findSymbol(operand_list[(function==OPCODE_JMP)?0:2].symbol, a->symbolTable);
      if(index == -1) {
        Fixup* fix = malloc(sizeof(Fixup));
        *fix = (Fixup){ftell(a->output)+4, NULL};
        addSymbol((Symbol){operand_list[(function==OPCODE_JMP)?0:2].symbol, 0, 0, NOTYPE, LOCAL, -1, fix}, a->symbolTable);
      }
      else if(a->symbolTable[index].section < 0){
        Fixup *new_fixup = malloc(sizeof(Fixup));       //
        new_fixup->offset = ftell(a->output) + 4;       //  Insertujemo ulancanu listu na
        new_fixup->next = a->symbolTable[index].fixup;  //  glavu liste posto nam je nebitan
        a->symbolTable[index].fixup = new_fixup;        //  poredak
      }
      instructionEmit(a->output, (Ins_emit){OC,M,regA,regB,regC,a->symbolTable[(index==-1)?0:index].value});
    }
    a->LC += 4;
    return PASS;

  case OPCODE_CALL:
  case OPCODE_LD:
  case OPCODE_ST:
  return PASS;

  case OPCODE_SUB:
    M = 0x01;
  case OPCODE_MUL:
    if(M == 0x00)  M = 0x02;
  case OPCODE_DIV:
    if(M == 0x00)  M = 0x03;
  case OPCODE_ADD:
    if(operand_count != 2)  return INVALID_OPERAND;
    if(operand_list[0].type != OPERAND_TYPE_REGISTER || operand_list[0].dereferance_flag != 0) return INVALID_OPERAND;
    if(operand_list[1].type != OPERAND_TYPE_REGISTER || operand_list[0].dereferance_flag != 0) return INVALID_OPERAND;
    regA = regB = registerIndex(operand_list[0].reg);
    regC = registerIndex(operand_list[1].reg);
    instructionEmit(a->output, (Ins_emit) {0x5, M, regA, regB, regC, 0});
    a->LC += 4;
    return PASS;

  case OPCODE_HALT:
    if(operand_count != 0)  return INVALID_OPERAND;
    emit4(a->output, 0);
    a->LC += 4;
    return PASS;

  case DIRECTIVE_EQU:
  case DIRECTIVE_ASCII:
  case DIRECTIVE_WORD:
  case DIRECTIVE_SKIP:
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
      if(index != -1) return EXTERN_SYMBOL_REDEFINITION;
      
      addSymbol(sym, a->symbolTable);
      a->symbolTable[index+1].binding = EXTERN;
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