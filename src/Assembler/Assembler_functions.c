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

  int symbolIndex = findSymbol(label, a->symbolTable);

  if(symbolIndex == -1){
    Symbol sym = (Symbol){label, a->LC, 0, NOTYPE, LOCAL, sectionIndex, NULL};
    addSymbol(sym , a->symbolTable);
  }
  else{
    if(a->symbolTable[symbolIndex].section >= 0)
      return DOUBLE_SYMBOL;
    if(a->symbolTable[symbolIndex].section == -2)
      return EXTERN_SYMBOL_REDEFINITION;
    a->symbolTable[symbolIndex].value = a->LC;
    a->symbolTable[symbolIndex].section = findSection(a->current_section, a->sectionTable);
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
    regA = registerIndex(operand_list[0].reg);
    regC = registerIndex(operand_list[1].reg);
    instructionEmit(a->output, (Ins_emit){7,byte,regA,regA, regC, 0});
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
    regA = registerIndex(operand_list[0].reg);
    regC = registerIndex(operand_list[1].reg);
    instructionEmit(a->output, (Ins_emit){6,byte,regA,regA, regC, 0});
    a->LC += 4;
    return PASS;

  case OPCODE_NOT:
    if(operand_count != 1)  return INVALID_OPERAND;
    if(operand_list[0].type != OPERAND_TYPE_REGISTER || operand_list[0].dereferance_flag != 0) return INVALID_OPERAND;
    regA = registerIndex(operand_list[0].reg);
    instructionEmit(a->output, (Ins_emit){6,0,regA,regA, 0, 0});
    a->LC += 4;
    return PASS;

  case OPCODE_BGT:
    if(M == 0x00)  M = 0x03;
  case OPCODE_BNE:
    if(M == 0x00)  M = 0x02;
  case OPCODE_BEQ:
    if(M == 0x00)  M = 0x01;
    if(operand_count != 3)  return INVALID_OPERAND;
    if(operand_list[2].dereferance_flag == 0) return INVALID_OPERAND;
    OC = 0x3;
    regA = 15;
    regB = registerIndex(operand_list[0].reg);
    regC = registerIndex(operand_list[1].reg);
  case OPCODE_JMP:
    if(function == OPCODE_JMP) {
      OC = 0x3;
      regA = 15;
      regB = 0;
      regC = 0;
      if(operand_count != 1)  return INVALID_OPERAND;
      if(operand_list[0].dereferance_flag == 0) return INVALID_OPERAND;
    }
    if(operand_list[(function==OPCODE_JMP)?0:2].type==OPERAND_TYPE_NUMBER){
      if(operand_list[(function==OPCODE_JMP)?0:2].number > 0x7FF ||
         operand_list[(function==OPCODE_JMP)?0:2].number < -(0x7FF + 1))
        return NUMBER_OVERFLOW;
      instructionEmit(a->output, (Ins_emit){OC,M,regA,regB,regC,operand_list[(function==OPCODE_JMP)?0:2].number} );
    }
    if(operand_list[(function==OPCODE_JMP)?0:2].type==OPERAND_TYPE_SYMBOL){
      index = findSymbol(operand_list[(function==OPCODE_JMP)?0:2].symbol, a->symbolTable);
      if(index == -1) {
        addSymbol((Symbol){operand_list[(function==OPCODE_JMP)?0:2].symbol, 0, 0, NOTYPE, LOCAL, -1, NULL}, a->symbolTable);
      }
      addRelocation((Relocation){findSection(a->current_section, a->sectionTable),
       findSymbol(operand_list[(function==OPCODE_JMP)?0:2].symbol, a->symbolTable),
       a->LC, 0, R_PC_12, 1}, a->relocTable);
      instructionEmit(a->output, (Ins_emit){OC,M,regA,regB,regC,(index==-1)?0:a->symbolTable[index].value});
    }
    a->LC += 4;
    return PASS;

  case OPCODE_CALL:
    if(operand_count != 1)  return INVALID_OPERAND;
    if(operand_list[0].type == OPERAND_TYPE_NUMBER){
      if(operand_list[0].dereferance_flag == 0) return INVALID_OPERAND;
      if(operand_list[0].number > 0x7FF ||
         operand_list[0].number < -(0x7FF + 1))
        return NUMBER_OVERFLOW;
      instructionEmit(a->output, (Ins_emit){0x2,15,0,0,0,operand_list[0].number});
    } else if(operand_list[0].type == OPERAND_TYPE_SYMBOL){
      index = findSymbol(operand_list[0].symbol, a->symbolTable);
      if(index == -1) {
        addSymbol((Symbol){operand_list[0].symbol, 0, 0, NOTYPE, LOCAL, -1, NULL}, a->symbolTable);
      }
      addRelocation((Relocation){findSection(a->current_section, a->sectionTable),
       findSymbol(operand_list[0].symbol, a->symbolTable),
       a->LC, 0, R_PC_12, 1}, a->relocTable);
      instructionEmit(a->output, (Ins_emit){0x2,15,0,0,0,(index==-1)?0:a->symbolTable[index].value});
    }
    a->LC += 4;
    return PASS;

  case OPCODE_ST:
  case OPCODE_LD:
    if(operand_count != 2)  return INVALID_OPERAND;
    if(operand_list[(function == OPCODE_LD)?1:0].type != OPERAND_TYPE_REGISTER)  return INVALID_OPERAND;

    char** reg = (function == OPCODE_LD)?&operand_list[1].reg:&operand_list[0].reg;
    Operand* operand = (function == OPCODE_LD)?&operand_list[0]:&operand_list[1];

    
    regA = registerIndex(*reg);
    OC = (function == OPCODE_LD)?0x9:0x8;

    if(operand->dereferance_flag == 0)
      M = (function == OPCODE_LD)?1:0;
    else
      M = 2;
    if(operand->type == OPERAND_TYPE_NUMBER){
      
      if(function == OPCODE_LD)
        instructionEmit(a->output, (Ins_emit){OC, M, regA, 0, 0, operand->number});
      else
        instructionEmit(a->output, (Ins_emit){OC, M, 0, 0, regA, operand->number});

    }else if(operand->type == OPERAND_TYPE_REGISTER){
    
      regB = registerIndex(operand->reg);
      if(function == OPCODE_LD)
        instructionEmit(a->output, (Ins_emit){OC, M, regA, regB, 0, 0});
      else
        instructionEmit(a->output, (Ins_emit){OC, M, 0, regB, regA, 0});

    }else if(operand->type == OPERAND_TYPE_SYMBOL){
      
      index = findSymbol(operand->symbol, a->symbolTable);
      if(index == -1){
        addSymbol((Symbol){operand->symbol, 0, 0, NOTYPE, LOCAL, -1, NULL}, a->symbolTable);
      }

      addRelocation((Relocation){
       findSection(a->current_section, a->sectionTable),
       findSymbol(operand->symbol, a->symbolTable),
       a->LC,
       0,
       (M == 2)?R_PC_12:R_ABS_12,
       1   }, a->relocTable);

      if(function == OPCODE_LD)
        instructionEmit(a->output, (Ins_emit){OC, M, regA, regB, 0, 0});
      else
        instructionEmit(a->output, (Ins_emit){OC, M, 0, regB, regA, 0});

    }else if(operand->type == OPERAND_TYPE_EXPRESSION){
      Expression* expression = operand->expression; 
      if(expression->arr_size != 3) return INVALID_OPERAND;
      if(expression->arr[0].type != ITEM_REGISTER) return INVALID_OPERAND;
      
      regB = registerIndex(expression->arr[0].reg);
    
      if(expression->arr[1].type == ITEM_NUMBER){
        eightbyte = expression->arr[1].number;
        
        if(expression->arr[2].op == OPERATION_TYPE_SUB)
          eightbyte = -eightbyte;
          
        
        if(function == OPCODE_LD)
          instructionEmit(a->output, (Ins_emit){OC, M, regA, regB, 0, eightbyte});
        else
          instructionEmit(a->output, (Ins_emit){OC, M, 0, regB, regA, eightbyte});
    
      }else if(expression->arr[1].type == ITEM_SYMBOL){

        index = findSymbol(expression->arr[1].symbol, a->symbolTable);
        if(index == -1){
          addSymbol((Symbol){expression->arr[1].symbol, 0, 0, NOTYPE, LOCAL, -1, NULL}, a->symbolTable);
        }
        
        
        addRelocation((Relocation){
          findSection(a->current_section, a->sectionTable),
          findSymbol(expression->arr[1].symbol, a->symbolTable),
          a->LC, 
          0, 
          (M == 2)?R_ABS_12:R_PC_12,
          (expression->arr[2].op == OPERATION_TYPE_SUB)?-1:1}, a->relocTable);
        
        if(function == OPCODE_LD)
          instructionEmit(a->output, (Ins_emit){OC, M, regA, regB, 0, 0});
        else
          instructionEmit(a->output, (Ins_emit){OC, M, 0, regB, regA, 0});
      }
    }
    a->LC += 4;
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
    instructionEmit(a->output, (Ins_emit){0,0,0,0,0,0});
    a->LC += 4;
    return PASS;

  case DIRECTIVE_EQU:
    if(operand_count != 2)   return INVALID_OPERAND;
    if(operand_list[0].type != OPERAND_TYPE_SYMBOL) return INVALID_OPERAND;
    index = findSymbol(operand_list[0].symbol, a->symbolTable);
    if(index == -1){
      addSymbol((Symbol){operand_list[0].symbol, 0, 0 , DATA, LOCAL, SECTION_TABLE_LENGHT, NULL}, a->symbolTable);
      index = findSymbol(operand_list[0].symbol, a->symbolTable);
    }
    
    Symbol* symbol0 = &a->symbolTable[index];
    
    if(operand_list[1].type == OPERAND_TYPE_NUMBER){
      int value = operand_list[1].number;
      
      symbol0->value = value;
      symbol0->section = SECTION_TABLE_LENGHT;
    } 
    else if(operand_list[1].type == OPERAND_TYPE_SYMBOL){
      int index1 = findSymbol(operand_list[1].symbol, a->symbolTable);

      if(index1 == -1){
        addSymbol((Symbol){operand_list[1].symbol, 0, 0 , DATA, LOCAL, -1, NULL}, a->symbolTable);
        index1 = findSymbol(operand_list[1].symbol, a->symbolTable);
      }

      ForwardDeclaration* fd = malloc(sizeof(ForwardDeclaration));
      fd->symbol = operand_list[1].symbol;
      fd->dependency = malloc(sizeof(int));
      fd->dependency[0] = index1;
      fd->count = 1;
      fd->type = OPERAND_TYPE_SYMBOL;

      symbol0->fd = fd;
      symbol0->section = -1;
    }else if(operand_list[1].type == OPERAND_TYPE_EXPRESSION){
      int symbol_count = 0, fd_size = 32;
      Expression* expression = operand_list[1].expression;
      
      ForwardDeclaration* fd = NULL;

      for(int i = 0; i < expression->arr_size; i++){
        if(expression->arr[i].type == ITEM_SYMBOL){
          symbol_count++;
          
          int index1 = findSymbol(expression->arr[i].symbol, a->symbolTable);
          
          if(index1 == -1){
            addSymbol((Symbol){expression->arr[i].symbol, 0, 0 , DATA, LOCAL, -1, NULL}, a->symbolTable);
            index1 = findSymbol(expression->arr[i].symbol, a->symbolTable);
          }

          if(fd == NULL){
            fd = malloc(sizeof(ForwardDeclaration));
            fd->dependency = malloc(fd_size* sizeof(int));
          }
          if(symbol_count-1 >= fd_size){
            fd_size *= 2;
            fd->dependency = realloc(fd->dependency, fd_size*sizeof(int));
          }
          fd->dependency[symbol_count-1] = index1;
          fd->count = symbol_count;

          symbol0->fd = fd;
        }
      }
      if(fd != NULL){
        fd->expression = expression;
        fd->type = OPERAND_TYPE_EXPRESSION;

        symbol0->fd = fd;
        symbol0->section = -1;
      }else{
        symbol0->value = evalExpression(expression, a);
        symbol0->section = SECTION_TABLE_LENGHT;
      }
    }else return INVALID_OPERAND;
    return PASS;

  case DIRECTIVE_ASCII:
    if(operand_count != 1)   return INVALID_OPERAND;
    if(operand_list[0].type != OPERAND_TYPE_STRING) return INVALID_OPERAND;
    eightbyte = strlen(operand_list[0].string);
    for(int i = 0; i < eightbyte; i++)
      emit1(a->output, operand_list[0].string[i]);
    a->LC += eightbyte;
    return PASS;

  case DIRECTIVE_WORD:
    if(a->current_section == NULL) return SYMBOL_NOT_IN_SECTION;
    for(int i = 0; i < operand_count; i++){
      if(operand_list[i].type == OPERAND_TYPE_NUMBER){
        emit4(a->output, operand_list[i].number);
      }else if(operand_list[i].type == OPERAND_TYPE_SYMBOL){
        index = findSymbol(operand_list[i].symbol, a->symbolTable);
        if(index == -1){
          addSymbol((Symbol){operand_list[i].symbol, 0, 0, NOTYPE, LOCAL, -1, NULL}, a->symbolTable);
        }
        emit4(a->output, (index==-1)?0:a->symbolTable[index].value);
        addRelocation((Relocation){findSection(a->current_section, a->sectionTable),
         operand_list[i].symbol , a->LC, 0, R_ABS_32, 1}, a->relocTable);
      }else return INVALID_OPERAND;
      a->LC+=4;
    }
    return PASS;

  case DIRECTIVE_SKIP:
    if(a->current_section == NULL) return SYMBOL_NOT_IN_SECTION;
    if(operand_count != 1)  return INVALID_OPERAND;
    if(operand_list[0].type != OPERAND_TYPE_NUMBER) return INVALID_OPERAND;
    for(int i = 0; i < operand_list[0].number; i++)
      emit1(a->output, 0);
    a->LC += operand_list[0].number;
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
      Symbol sym = {operand_list[i].symbol, 0, 0, NOTYPE, GLOBAL, -1};
      //findSection(a->current_section, a->sectionTable)
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