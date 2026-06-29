#include "../../inc/Emulator/Emulator.hpp"

int main(int argc, const char** argv){
  
  if(argc != 2){
    printf("\nEMULATOR: Error unknown option only one executable file allowed!\n");
    return 1;
  }

  FILE* input = fopen(argv[1], "r");

  uint64_t signature = 0;
  fseek(input, 0, SEEK_SET);
  fread(&signature, 8, 1, input);

  if(signature != 0x3436464C455F5300){
    printf("\nEMULATOR: Bad signature only executable files allowed!\n");
    return 2;
  }

  signature = 0;

  fseek(input, 0xC, SEEK_SET);
  fread(&signature, 4, 1, input);

  if(signature != 0){
    printf("\nEMULATOR: Only executable files allowed no relocatable :( !\n");
    return 3;
  }

  Emulator(input);

  fclose(input);

  return 0;
}