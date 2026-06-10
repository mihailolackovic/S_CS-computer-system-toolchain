#include "../../inc/Assembler/Assemble.h"
int debug = 1;
void Assemble(Instruction* instructions, int instruction_count, FILE* output){

  Symbol symbolTable[SYMBOL_TABLE_LENGHT] = {0};
  Section sectionTable[SECTION_TABLE_LENGHT] = {0};
  Relocation relocTable[RELOC_TABLE_LENGHT] = {0};

  Assembler_state a = {output, 1, 0, 0, NULL, symbolTable, sectionTable, relocTable, NULL};

  int index = 0;
  AsmERROR error = PASS;
  
  emit8(a.output, 0x00535F454C463634);    // S_ELF signature
  emit8(a.output, 0x0000000000000000);  // last fourbyte = SectionHeaderTable ptr
  
  while(a.assembler_flag){
    Instruction* ins = &instructions[index];
    
    Functions func = lookup_function(ins->function);
    
    if(ins->label)
      if((error = labelSymbol(ins->label, &a)) != PASS)
        handleEXIT(error, ins);

    if((error = execute_function(func, ins->operand_list, ins->operand_count, &a)) != PASS)
      handleEXIT(error, ins);
    
    if(index == instruction_count-1)  a.assembler_flag = 0;
    else                              index++;
  }

  resolveFD(&a);

  resolvePCRel(&a);

  emitSymsTable(&a);
  emitRelocTable(&a);
  emitSectionHeader(&a);

  fseek(a.output, 0xC, SEEK_SET);
  fwrite(&a.headerOffset, 4, 1, a.output);

  if(debug == 1){
    printSectionTable(&a);
    printSymsTable(&a);
    printRelocTable(&a);
  }
  return;
};

void emitSymsTable(Assembler_state* a){
  int Sym_Pool_size = 0;
  int Sym_Table_size = 0;
  int sym_name_offset[SYMBOL_TABLE_LENGHT];
  for(int i = 0; i < SYMBOL_TABLE_LENGHT; i++){
    if(a->symbolTable[i].name){
      sym_name_offset[i] = Sym_Pool_size;
      Sym_Pool_size += strlen(a->symbolTable[i].name)+1;
      for(int j = 0; j < strlen(a->symbolTable[i].name); j++)
        emit1(a->output, a->symbolTable[i].name[j]);
      emit1(a->output, 0);
    } else break;
  }
  for(int i = 0; i < SYMBOL_TABLE_LENGHT; i++){
    if(a->symbolTable[i].name){
      Sym_Table_size += 16;
      emit4(a->output, sym_name_offset[i]);
      emit4(a->output, a->symbolTable[i].value);
      emit4(a->output, a->symbolTable[i].size);
      emit1(a->output, a->symbolTable[i].type);
      emit1(a->output, a->symbolTable[i].binding);
      emit2(a->output, a->symbolTable[i].section);
    } else break;
  }
  if(a->sectionTable[SECTION_TABLE_LENGHT-5].name != NULL)
    return;
  addSection((Section){"Sym_pool", 0, Sym_Pool_size}, a->sectionTable);
  addSection((Section){"Sym_table", 0, Sym_Table_size}, a->sectionTable);
};

void emitRelocTable(Assembler_state* a){
  int reloc_table_size = 0;
  for(int i = 0; i < RELOC_TABLE_LENGHT; i++){
    if(a->relocTable[i].type != UNINITIALISED){
      emit4(a->output, a->relocTable[i].section);
      emit4(a->output, a->relocTable[i].symbol);
      emit4(a->output, a->relocTable[i].offset);
      emit4(a->output, a->relocTable[i].addend);
      emit1(a->output, a->relocTable[i].type);
      emit3(a->output, 0);
      reloc_table_size += 20;
    }else break;
  }
  addSection((Section){"Rel_Table", 0, reloc_table_size}, a->sectionTable);
};

AsmERROR emitSectionHeader(Assembler_state* a){
  int sec_Pool_size = 0;
  int sec_name_offset[SECTION_TABLE_LENGHT];
  int header_size = 0x10;
  int cursor = header_size;
  for(int i = 0; i < SECTION_TABLE_LENGHT; i++){
    if(a->sectionTable[i].name != NULL){
      sec_name_offset[i] = sec_Pool_size;
      sec_Pool_size += strlen(a->sectionTable[i].name)+1;
      for(int j = 0; j < strlen(a->sectionTable[i].name); j++)
        emit1(a->output, a->sectionTable[i].name[j]);
      emit1(a->output, 0);
    } else break;
  }
  addSection((Section){"Sect_Pool", 0, sec_Pool_size}, a->sectionTable);
  a->headerOffset = ftell(a->output);
  for(int i = 0; i < SECTION_TABLE_LENGHT; i++){
    if(a->sectionTable[i].name != NULL){
      a->sectionTable[i].base = cursor;
      cursor += a->sectionTable[i].lenght;
      emit4(a->output ,sec_name_offset[i]);
      emit4(a->output ,a->sectionTable[i].lenght);
      emit4(a->output ,a->sectionTable[i].base);
    } else break;
  }
  return PASS;
};

AsmERROR resolvePCRel(Assembler_state* a){
  for(int i = 0; i < RELOC_TABLE_LENGHT; i++){
    if(a->relocTable[i].type == UNINITIALISED) break;
    
    if(a->relocTable[i].type == R_ABS_12){

      if(a->symbolTable[a->relocTable[i].symbol].section == ABS_S){

        int number = a->symbolTable[a->relocTable[i].symbol].value;
        
        if(number > 0x7FF || number < -(0x7FF + 1))
          return NUMBER_OVERFLOW;

        int offset = 0x10;
        for(int j = 0; j < SECTION_TABLE_LENGHT; j++){
          if(a->sectionTable[j].name != NULL){
            if(a->relocTable[i].section == j){
              offset +=  a->relocTable[i].offset;
              break;
            } else{
              offset +=  a->sectionTable[j].lenght;
            }
          }
        }
        long stara_pozicija = ftell(a->output);
        uint32_t ins;
        fseek(a->output, offset, SEEK_SET);
        fread(&ins, 4, 1, a->output);

        ins &= 0xFFFFF000;
        ins |= (number & 0x0FFF);

        fseek(a->output, offset, SEEK_SET);
        fwrite(&ins, 4, 1, a->output);

        fseek(a->output, stara_pozicija, SEEK_SET);
        a->relocTable[i].type = RESOLVED;
      }

    } else if(a->relocTable[i].type == R_PC_12){
      if(a->symbolTable[a->relocTable[i].symbol].section == a->relocTable[i].section){
        int number = a->symbolTable[a->relocTable[i].symbol].value - a->relocTable[i].offset;
        
        if(number > 0x7FF || number < -(0x7FF + 1))
          return NUMBER_OVERFLOW;

        int offset = 0x10;
        for(int j = 0; j < SECTION_TABLE_LENGHT; j++){
          if(a->sectionTable[j].name != NULL){
            if(a->relocTable[i].section == j){
              offset +=  a->relocTable[i].offset;
              break;
            } else{
              offset +=  a->sectionTable[j].lenght;
            }
          }
        }
        long stara_pozicija = ftell(a->output);
        uint32_t ins;
        fseek(a->output, offset, SEEK_SET);
        fread(&ins, 4, 1, a->output);

        ins &= 0xFFFFF000;
        ins |= (number & 0x0FFF);

        fseek(a->output, offset, SEEK_SET);
        fwrite(&ins, 4, 1, a->output);

        fseek(a->output, stara_pozicija, SEEK_SET);
        a->relocTable[i].type = RESOLVED;
      }
    }
  }
};


AsmERROR resolveFD(Assembler_state* a){
  // Ovde bi bilo pametno da se napravi grafik zavisnosti 
  // pa da se pretrazuje za cikluse i DFS al zajebi to :( da u C-u radim, neki drugi put
  int usl[SYMBOL_TABLE_LENGHT] = {0};
  int usld[SYMBOL_TABLE_LENGHT][SYMBOL_TABLE_LENGHT] = {0};
  int usls = 0;
  int uslds[SYMBOL_TABLE_LENGHT] = {0};

  char fail = 0;

  for(int i = 0; i < SYMBOL_TABLE_LENGHT; i++){
    if(a->symbolTable[i].fd == NULL)  continue;

    ForwardDeclaration* fd = a->symbolTable[i].fd;
    for(int j = 0; j < fd->count; j++){
      if(a->symbolTable[fd->dependency[j]].section == -1){
        fail = 1;
        break;
      }
    }
    if(fail == 1){
      usl[usls] = findSymbol(a->symbolTable[i].name, a->symbolTable);
      for(int j = 0; j < fd->count; j++){
        usld[usls][uslds[usls]++] = fd->dependency[j];  // :(
      }
      usls++;
      fail = 0;
      continue;
    }

    int relocability = evalRelocability(fd->expression, a);

    if(relocability == -1) return EXPRESSION_NOT_RELOCATABLE;

    if(relocability == ABS_S){
      a->symbolTable[i].value = evalExpression(fd->expression, a);
      a->symbolTable[i].section = ABS_S;
    } else {
      addRelocation((Relocation){}, a->relocTable);
      a->symbolTable[i].section = relocability;
    }
  }

  char progress_flag = 1;
  
  while (progress_flag) {
    progress_flag = 0;

    for (int i = 0; i < usls; i++) {
      int index = usl[i];

      if (a->symbolTable[index].section != -1) {
        continue;
      }

      char completed_flag = 1;

      for (int j = 0; j < uslds[i]; j++) {
        int zavisni_simbol_idx = usld[i][j];
        
        if (a->symbolTable[zavisni_simbol_idx].section == -1) {
          completed_flag = 0;
          break;
        }
      }

      if (completed_flag) {
        ForwardDeclaration* fd = a->symbolTable[index].fd;

        int relocability = evalRelocability(fd->expression, a);
        if (relocability == -1) return EXPRESSION_NOT_RELOCATABLE;

        a->symbolTable[index].value = evalExpression(fd->expression, a);
        a->symbolTable[index].section = relocability;

        progress_flag = 1; 
      }
    }
  }
  for (int i = 0; i < usls; i++) {
    int check = usl[i];
    if (a->symbolTable[check].section == -1) {
      return CYCLICAL_DEPENDENCY;
    }
  }

  return PASS;
}

AsmERROR handleEXIT(AsmERROR error, Instruction* ins){
  printf("_______ERROR_________ TYPE: %d ___________\n", error);
  printInstruction(*ins);
  if(debug == 1)
    return;
  else
    exit(error);
};

void printInstruction(Instruction ins){
  printf("%s  ", (ins.label)?ins.label:"NO_LABEL");
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
  printf("\n"); debug = 0;
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
      printf("Name: %s | Value: %d | Size: %d | Type: %s | Binding: %s | Section: %s\n",
      a->symbolTable[i].name, a->symbolTable[i].value, a->symbolTable[i].size, type,
       (a->symbolTable[i].binding == 0)?"LOCAL":"GLOBAL", section);
    }else
      return;
  }
};


void printRelocTable(Assembler_state* a){
  char* type;
  for(int i = 0; i < RELOC_TABLE_LENGHT; i++){
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