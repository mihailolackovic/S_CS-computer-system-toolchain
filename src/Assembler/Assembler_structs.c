#include "../../inc/Assembler/Assembler_structs.h"

int addSection(Section section, Section* sectionTable){
  for(int i = 0; i < SECTION_TABLE_LENGHT; i++){
    if(sectionTable[i].name){
      if(strcmp(section.name,sectionTable[i].name) == 0) return -1;
    }else  {
      sectionTable[i] = section;
      return i;
    }
  }
};
int findSection(char* section, Section* sectionTable){
  for(int i = 0; i < SECTION_TABLE_LENGHT; i++){
    if(sectionTable[i].name){
      if(strcmp(section, sectionTable[i].name) == 0) 
        return i;
    } else
      break;
  }
  return -1;
};

AsmERROR addSymbol(Symbol symbol, Symbol* symbolTable){
  for(int i = 0; i < SYMBOL_TABLE_LENGHT; i++){
    if(symbolTable[i].name){
      if(strcmp(symbol.name, symbolTable[i].name) == 0) return DOUBLE_SYMBOL;
      if(symbol.type == EXTERN) return EXTERN_SYMBOL_REDEFINITION;
    }else  {
      symbolTable[i] = symbol;
      return PASS;
    }
  }
  return TABLE_FULL;
};

int findSymbol(char* symbol, Symbol* symbolTable){
  for(int i = 0; i < SYMBOL_TABLE_LENGHT; i++){
    if(symbolTable[i].name){
      if(strcmp(symbol, symbolTable[i].name) == 0) 
        return i;
    }else
      break;
  }
  return -1;
};

int registerIndex(char* reg){
  switch (reg[0])
  {
  case 'r':
    return atoi(reg+1);
  case 's':
    if(reg[1] == 'p') return 14;
    return 0;
  case 'h':
    return 1;
  case 'c':
    return 2;
  case 'p':
    return 15;
  default:
    return -1;
  }
};


AsmERROR addRelocation(Relocation relocation, Relocation* relocationTable){
  for(int i = 0; i < RELOC_TABLE_LENGHT; i++){
    if(!relocationTable[i].type){
      relocationTable[i] = relocation;
      return PASS;
    }
  }
  return TABLE_FULL;
};

int evalExpression(Expression* exp, Assembler_state* a){
  StackItem* expression = exp->arr;
  int stack_size  = exp->arr_size;
  
  int* stack = malloc(stack_size*sizeof(int));
  int top = -1;

  StackItem x;
  int rez, y, z;

  for(int i = 0; i < stack_size; i++){
    if(top < -1)   goto error;
    x = expression[i];

    if(x.type == ITEM_NUMBER){
    
      stack[++top] = x.number;
    
    } else if(x.type == ITEM_SYMBOL){
    
      stack[++top] = a->symbolTable[findSymbol(x.symbol, a->symbolTable)].value;

    } else if(x.type == ITEM_OPERATION){
      if(top < 1)   goto error;

      y = stack[top--];
      z = stack[top--];

      switch (x.op)
      {
      case OPERATION_TYPE_ADD:
        rez = y + z;
        break;
      case OPERATION_TYPE_SUB:
        rez = z - y;
        break;
      case OPERATION_TYPE_MUL:
        rez = y * z;
        break;
      case OPERATION_TYPE_DIV:
        if(y == 0)   goto error;
        rez = z / y;
        break;
      default:
        goto error;
      }

      stack[++top] = rez;
    }
  }
  rez = stack[top--];
  
  if(top != -1) goto error;
  
  return rez;

error:
  free(stack);
  return -9999;
};


int evalRelocability(Expression* exp, Assembler_state* a) {
  StackItem* expression = exp->arr;
  int stack_size = exp->arr_size;

  RelocStackItem* stack = calloc(stack_size, sizeof(RelocStackItem));
  int top = -1;

  int next_unique_section = ABS_S + 1; // Jedinsvene sekcije za ekserne simbole 

  for (int i = 0; i < stack_size; i++) {
    StackItem x = expression[i];

    if (x.type == ITEM_NUMBER) {

      top++;
      memset(stack[top].section_counts, 0, sizeof(stack[top].section_counts));

    } 
    else if (x.type == ITEM_SYMBOL) {
      
      top++;
      memset(stack[top].section_counts, 0, sizeof(stack[top].section_counts));
      
      int index = findSymbol(x.symbol, a->symbolTable);
      
      if (a->symbolTable[index].section == EXTERN_S) {
        int unique_sec = next_unique_section++;
        if (unique_sec < 2*SECTION_TABLE_LENGHT) {
          stack[top].section_counts[unique_sec] = 1;
        }
      } else {
      int section = a->symbolTable[index].section;
        if (section != SECTION_TABLE_LENGHT && section >= 0 && section < 2*SECTION_TABLE_LENGHT)
          stack[top].section_counts[section] = 1;
      }
    } 
    else if (x.type == ITEM_OPERATION) {
      if (top < 1) goto error1;

      RelocStackItem y = stack[top--];
      RelocStackItem z = stack[top--];
      RelocStackItem rez;
      memset(rez.section_counts, 0, sizeof(rez.section_counts));

      char y_abs = 1;
      char z_abs = 1;
      switch (x.op) {
        case OPERATION_TYPE_ADD:
          for (int s = 0; s < 2*SECTION_TABLE_LENGHT; s++)
            rez.section_counts[s] = z.section_counts[s] + y.section_counts[s];

          break;

        case OPERATION_TYPE_SUB:
          for (int s = 0; s < 2*SECTION_TABLE_LENGHT; s++)
            rez.section_counts[s] = z.section_counts[s] - y.section_counts[s];

          break;

        case OPERATION_TYPE_MUL:
        case OPERATION_TYPE_DIV:
            
          y_abs = 1;
          z_abs = 1;
          for (int s = 0; s < 2*SECTION_TABLE_LENGHT; s++) {
            if (y.section_counts[s] != 0) y_abs = 0;
            if (z.section_counts[s] != 0) z_abs = 0;
          }

          if (!y_abs && !z_abs) goto error1;

          if (x.op == OPERATION_TYPE_MUL) {
              
            for (int s = 0; s < 2*SECTION_TABLE_LENGHT; s++) 
              rez.section_counts[s] = z.section_counts[s] + y.section_counts[s]; 
            
          } else {
            if (!y_abs) goto error1;
            for (int s = 0; s < 2*SECTION_TABLE_LENGHT; s++)
              rez.section_counts[s] = z.section_counts[s];
          }
          break;

        default:
          goto error1;
      }
      stack[++top] = rez;
    }
  }
  
  if (top != 0) goto error1;

  RelocStackItem final_rez = stack[top--];
  free(stack);

  int active_section = ABS_S;
  int non_zero_sections_count = 0;

  for (int s = 0; s < 2*SECTION_TABLE_LENGHT; s++) {
    if (final_rez.section_counts[s] != 0) {

      if (final_rez.section_counts[s] == 1) {
        non_zero_sections_count++;
        active_section = s;
      } else {

        return -1; 
      }
    }
  }

  if (non_zero_sections_count == 0) {
  
    return ABS_S;
  
  } else if (non_zero_sections_count == 1) {

    if (active_section >= SECTION_TABLE_LENGHT) return -1; 
    
    return active_section;
  }

  return -1;
   
error1:
  free(stack);
  return -1;
}