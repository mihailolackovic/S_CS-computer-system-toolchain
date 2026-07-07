#include "../../inc/Assembler/Assemble.h"

//#define DEBUG_ASM

void Assemble(Instruction* instructions, int instruction_count, FILE* output){
  Symbol symbolTable[SYMBOL_TABLE_LENGHT] = {0};
  Section sectionTable[SECTION_TABLE_LENGHT] = {0};
  Relocation relocTable[RELOC_TABLE_LENGHT] = {0};

  Assembler_state a = {output, 1, -1, 0, 0, NULL, symbolTable, sectionTable, relocTable, NULL, 0};

  int index = 0;
  AsmERROR error = PASS;
  
  emit8(a.output, 0x00535F454C463634);    // S_ELF signature
  emit8(a.output, 0x0000000000000000);    // last fourbyte = SectionHeaderTable ptr
  
  while(a.assembler_flag){
    Instruction* ins = &instructions[index];
    
    Functions func = lookup_function(ins->function);
    
    if(ins->label)
      if((error = labelSymbol(ins->label, func, &a)) != PASS)
        handleEXIT(error, ins);

    if((error = execute_function(func, ins->operand_list, ins->operand_count, &a)) != PASS)
      handleEXIT(error, ins);
    
    if(index == instruction_count-1)  a.assembler_flag = 0;
    else                              index++;
  }

  resolveFD(&a);

  resolveReloc(&a);

  emitSymsTable(&a);
  emitRelocTable(&a);
  emitSectionHeader(&a);

  fseek(a.output, 0xC, SEEK_SET);
  fwrite(&a.headerOffset, 4, 1, a.output);

#ifdef DEBUG_ASM
  printSectionTable(&a);
  printSymsTable(&a);
  printRelocTable(&a);
#endif

  return;
};

AsmERROR resolveReloc(Assembler_state* a){
  for(int i = 0; i < RELOC_TABLE_LENGHT; i++){
    if(a->relocTable[i].type == UNINITIALISED) break; // footer relocTable

    if(a->relocTable[i].type == R_ABS_12){
      if(a->symbolTable[a->relocTable[i].symbol].section == ABS_S){
        int number = a->symbolTable[a->relocTable[i].symbol].value;

        if(number > 0x7FF || number < -(0x7FF + 1)) //  Nek se linker muci sa ovim
          continue;                                 //

        int offset = 0x10;
        for(int j = 0; j < SECTION_TABLE_LENGHT; j++){
          if(a->sectionTable[j].name != NULL){
            if(a->relocTable[i].section == j){
              offset += a->relocTable[i].offset;
              break;
            } else {
              offset += a->sectionTable[j].lenght;
            }
          }
        }
        long stara_pozicija = ftell(a->output);
        uint32_t ins;
        fseek(a->output, offset, SEEK_SET);
        fread(&ins, 4, 1, a->output);

        int opcode = (ins >> 28) & 0xF;
        int regA = (ins >> 20) & 0xF;
        int regC = (ins >> 12) & 0xF;
        if(opcode == 0x9){
          // ld immediate: gpr[A] = r0 + D  (M=1, B=0, C=0)
          ins = (0x9u << 28) | (0x1u << 24) | ((uint32_t)regA << 20) | (number & 0x0FFF);
        } else if(opcode == 0x8){
          // st immediate: mem[r0+r0+D] = gpr[C]  (M=0, A=0, B=0, keep C)
          ins = (0x8u << 28) | (0x0u << 24) | ((uint32_t)regC << 12) | (number & 0x0FFF);
        } else {
          // nije ld/st nebi trebalo se desiti al u slucaju modfikacije :)
          ins &= 0xFFFFF000;
          ins |= number;
        }

        fseek(a->output, offset, SEEK_SET);
        fwrite(&ins, 4, 1, a->output);

        fseek(a->output, stara_pozicija, SEEK_SET);
        a->relocTable[i].type = RESOLVED;
      }
    } else if(a->relocTable[i].type == R_PC_12){
      if(a->symbolTable[a->relocTable[i].symbol].section == a->relocTable[i].section){
        int number = a->symbolTable[a->relocTable[i].symbol].value - a->relocTable[i].offset - 4;
        if(number > 0x7FF || number < -(0x7FF + 1))
          continue;
        int offset = 0x10;
        for(int j = 0; j < SECTION_TABLE_LENGHT; j++){
          if(a->sectionTable[j].name != NULL){
            if(a->relocTable[i].section == j){
              offset += a->relocTable[i].offset;
              break;
            } else {
              offset += a->sectionTable[j].lenght;
            }
          }
        }
        long stara_pozicija = ftell(a->output);
        uint32_t ins;
        fseek(a->output, offset, SEEK_SET);
        fread(&ins, 4, 1, a->output);

        int opcode = (ins >> 28) & 0xF;

        if(opcode == 0x9 || opcode == 0x8){           // ostavljamo linkeru i ovu goadost
          fseek(a->output, stara_pozicija, SEEK_SET); //
          continue;                                   //
        }

        if(opcode == 0x3){
          uint32_t mod = (ins >> 24) & 0xF;
          mod &= 0x7;
          ins = (ins & 0xF0FFF000) | (mod << 24) | (number & 0x0FFF);
        } else if(opcode == 0x2){
          ins = (ins & 0xF0FFF000) | (0x0 << 24) | (number & 0x0FFF);
        } else {
          ins &= 0xFFFFF000;
          ins |= (number & 0x0FFF);
        }

        fseek(a->output, offset, SEEK_SET);
        fwrite(&ins, 4, 1, a->output);
        fseek(a->output, stara_pozicija, SEEK_SET);
        a->relocTable[i].type = RESOLVED;
      }
    }
  }
  return PASS;
};


AsmERROR resolveFD(Assembler_state* a){
  // Ovde bi bilo pametno da se napravi grafik zavisnosti 
  // pa da se pretrazuje za cikluse i DFS al zajebi to :( da u C-u radim, neki drugi put
  int usl[SYMBOL_TABLE_LENGHT] = {0};                       //  Unresolved symbols list
  int usld[SYMBOL_TABLE_LENGHT][SYMBOL_TABLE_LENGHT] = {0}; //  USL dependency
  int usls = 0;                                             //  USL size(count)
  int uslds[SYMBOL_TABLE_LENGHT] = {0};                     //  USLD size(count)

  char fail = 0;

  for(int i = 0; i < SYMBOL_TABLE_LENGHT; i++){
    if(a->symbolTable[i].fd == NULL)  continue;

    ForwardDeclaration* fd = a->symbolTable[i].fd;
    for(int j = 0; j < fd->count; j++){
      if(a->symbolTable[fd->dependency[j]].section == -1){
        fail = 1;                                //  Ako simbol zavisi od simbola cija sekcija
        break;                                   //   je nepoznata moramo to da snimimo za kasnije
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
    // Ako za sve simbole od kojih zavisi nas simbol znamo odakle su mozemo da ih razresimo
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

  char progress_flag = 1; // Sve dok napredujemo znaci da razresavamo simbole koji nisu kruzno
                          // kruzno zavisni jer nam je tako magicni algoritam rekao ;)
  while (progress_flag) { // Pokusavamo da razresimo sve simbole koji su ostali ne razreseni
    progress_flag = 0;    // a realno mozemo

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
  char* errorArray[] = {"PASS", "TABLE_FULL", "INVALID_OPERAND", "DOUBLE_SYMBOL", "DOUBLE_SECTION"
  , "EXTERN_SYMBOL_REDEFINITION", "SYMBOL_NOT_IN_SECTION", "NUMBER_OVERFLOW", "INVALID_EXPRESSION"
  , "EXPRESSION_NOT_RELOCATABLE", "CYCLICAL_DEPENDENCY", "LITERAL_INSERTION"};
  printf("_______ERROR_________ TYPE: %s ___________\nLINE :    ", errorArray[error]);
  printInstruction(*ins);

#ifdef DEBUG_ASM
  return;
#endif

  exit(error);
};
