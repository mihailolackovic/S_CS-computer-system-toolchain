#include "../../inc/Assembler/Assembler_debug.h"


void printInstruction(Instruction ins){
  printf("%s:  ", (ins.label)?ins.label:"NO_LABEL");
  printf("%s  ", ins.function);
  char c;
  for(int j = 0; j < ins.operand_count; j++){
    switch (ins.operand_list[j].type){
    case OPERAND_TYPE_EXPRESSION:
      printf("(");
      for(int i = 0; i < ins.operand_list[j].expression->arr_size; i++){
        switch (ins.operand_list[j].expression->arr[i].type){
        case ITEM_NUMBER:
          printf(" %d ", ins.operand_list[j].expression->arr[i].number);
          break;
        case ITEM_SYMBOL:
          printf(" %s ", ins.operand_list[j].expression->arr[i].symbol);
          break;
        case ITEM_REGISTER:
          printf(" %%%s ", ins.operand_list[j].expression->arr[i].reg);
          break;
        case ITEM_OPERATION:
          switch(ins.operand_list[j].expression->arr[i].op){
          case 1:
            c = '+';
            break;
          case 2:
            c = '-';
            break;
          case 3:
            c = '*';
            break;
          case 4:
            c = '/';
            break;
          }
          printf(" %c ", c);
          break;
        default:
          break;
        }
      }
      printf(")");
      break;
    typedef struct Assembler_state Assembler_state;
    case OPERAND_TYPE_STRING:
      printf("%s  ", ins.operand_list[j].string);
      break;
    case OPERAND_TYPE_SYMBOL:
      printf("%s  ", ins.operand_list[j].symbol);
      break;
    case OPERAND_TYPE_NUMBER:
      printf("%d  ", ins.operand_list[j].number);
      break;
    case OPERAND_TYPE_REGISTER:
      printf("%s  ", ins.operand_list[j].reg);
      break;
    default:
      break;
    }
  }
  printf("\n");
};

void printSectionTable(Assembler_state* a){
  for(int i = 0; i < SECTION_TABLE_LENGHT; i++){
    if(a->sectionTable[i].name)
        printf("Name: %s | Size: %d | Base: %d\n", a->sectionTable[i].name, a->sectionTable[i].lenght, a->sectionTable[i].base);
    else return;
  }
}

void printSymsTable(Assembler_state* a){
  for(int i = 0; i < SYMBOL_TABLE_LENGHT; i++){
    if(a->symbolTable[i].name){
      char* type;
      char* section;
      switch (a->symbolTable[i].section)
      {
      case -2:
        section = "EXTERN";
        break;
      case -1:
        section = "UND";
        break;
      case SECTION_TABLE_LENGHT:
        section = "*ABS*";
        break;
      
      default:
        section = a->sectionTable[a->symbolTable[i].section].name;
        break;
      }
      switch (a->symbolTable[i].type){
      case NOTYPE:
        type = "NOTYPE";
        break;
      case DATA:
        type = "DATA";
        break;
      case STR:
        type = "STR";
        break;
      case FUNC:
        type = "FUNC";
        break;
      case SECT:
        type = "SECT";
        break;
      case SRC:
        type = "SRC";
        break;
      }
      char* bind;
      switch(a->symbolTable[i].binding){
        case LOCAL:  bind = "LOCAL";  break;
        case GLOBAL: bind = "GLOBAL"; break;
        case EXTERN: bind = "EXTERN"; break;
        default:     bind = "?";      break;
      }
      printf("Name: %s | Value: %d | Size: %d | Type: %s | Binding: %s | Section: %s\n",
      a->symbolTable[i].name, a->symbolTable[i].value, a->symbolTable[i].size, type,
       bind, section);
    }else
      return;
  }
};


void printRelocTable(Assembler_state* a){
  
  for(int i = 0; i < RELOC_TABLE_LENGHT; i++){
    char* type;
    switch (a->relocTable[i].type)
    {
    case UNINITIALISED:
      return;
    case R_ABS_12:
      type = "R_ABS_12";
      break;
    case R_PC_12:
      type = "R_PC_12";
      break;
    case R_ABS_32:
      type = "R_ABS_32";
      break;
    case R_PC_32:
      type = "R_PC_32";
      break;
    case RESOLVED:
      type = "RESOLVED";
      break;
    default:
      return;
    }
    printf("Section: %s | Symbol: %s | Type: %s | Offset: %d | Addend: %d | Multiplier %d\n", 
     a->sectionTable[a->relocTable[i].section].name,
     a->symbolTable[a->relocTable[i].symbol].name, type,
     a->relocTable[i].offset, a->relocTable[i].addend, a->relocTable[i].multiplier);
  }
};