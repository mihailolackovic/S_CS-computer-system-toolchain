#include "../../inc/Assembler/Assemble.h"

void Assemble(Instruction* instructions, int instruction_count, FILE* output){
  Symbol symbolTable[30] = {0};
  Section sectionTable[10] = {0};
  Section abs = {"*ABS*", 0, 0};
  Assembler_state a = {output, 1, NULL, 0, symbolTable, sectionTable};
  int index = 0;
  AsmERROR error = PASS;
  
  emit8(a.output, 0x535F454C463634);    // S_ELF signature
  emit8(a.output, 0x0000000000000000);  // last byte = SectionHeaderTable ptr
  
  while(a.assembler_flag){
    Instruction* ins = &instructions[index];
    
    Functions func = lookup_function(ins->function);
    
    if(error = execute_function(func, ins->operand_list, ins->operand_count, &a) != PASS)
      handleEXIT(error);

    printInstruction(*ins);
    
    if(index == instruction_count-1)  a.assembler_flag = 0;
    else                              index++;
  }
  printSectionTable(&a);
  printSymsTable(&a);
  return;
};

AsmERROR handleEXIT(AsmERROR error){

};


void printInstruction(Instruction ins){
  printf("%s  ", (ins.label)?ins.label:"NO_LABEL");
  printf("%s  ", ins.function);
  for(int j = 0; j < ins.operand_count; j++){
    switch (ins.operand_list[j].type)
    {
    case OPERAND_TYPE_EXPRESSION:
      for(int k = 0; k < ins.operand_list[j].expression->arr_size; k++){
        char c = ' ';
        switch (ins.operand_list[j].expression->arr[k].operation)
        {
        case OPERATION_TYPE_ADD:
          c = '+';
          break;
        case OPERATION_TYPE_SUB:
          c = '-';
          break;
        case OPERATION_TYPE_MUL:
          c = '*';
          break;
        case OPERATION_TYPE_DIV:
          c = '/';
          break;
        case OPERATION_TYPE_NONE:
          c = ' ';
          break;
        }
        if(ins.operand_list[j].expression->arr[k].symbol)
          printf("(%d*%s)%c",ins.operand_list[j].expression->arr[k].coeficient,ins.operand_list[j].expression->arr[k].symbol, c);
        else
          printf("%d%c",ins.operand_list[j].expression->arr[k].coeficient,c);
      }
typedef struct Assembler_state Assembler_state;
      break;
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
  for(int i = 0; i < 30; i++){
    if(a->sectionTable[i].name)
        printf("Name: %s | Size: %d | Offset: %d\n", a->sectionTable[i].name, a->sectionTable[i].lenght, a->sectionTable[i].base);
    else return;
  }
}

void printSymsTable(Assembler_state* a){
  for(int i = 0; i < 30; i++){
    if(a->symbolTable[i].name){
      char* type;
      switch (a->symbolTable[i].type)
      {
      case NOTYPE:
        type = "NOTYPE";
        break;
      case DATA:
        type = "DATA";
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
      printf("Name: %s| Value: %d | Size: %d | Type: %s | Binding: %s | Section: %s\n",
      a->symbolTable[i].name, a->symbolTable[i].value, a->symbolTable[i].size, type, (a->symbolTable[i].binding)?"LOCAL":"GLOBAL", (a->symbolTable[i].section)?a->symbolTable[i].section->name:"UND");
    }else
      return;
  }
};