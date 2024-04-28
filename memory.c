#include <stdio.h>
#include <stdlib.h>

#define MAX_16 65536

typedef unsigned char* memory;
typedef unsigned short address;
typedef unsigned char byte;

memory create_RAM() {
  memory RAM = (memory) malloc(sizeof(char) * MAX_16);
  // Testing for enough memory
  if (RAM == NULL) {
    fprintf(stdout, "Not enough memory!\n");
    return NULL;
  }
  // Initializing RAM
  for (int i = 0; i < MAX_16; i++) {
    *(RAM + i) = 0x00;
  }
  // Return the newly created RAM
  return RAM;
}

void write(memory mem, address addr, byte data) {
  if (mem == NULL) {
    fprintf(stdout, "RAM not created!\n");
    return;
  }
  *(mem + addr) = data;
  return;
}

byte read(memory mem, address addr) {
  if (mem == NULL) {
    fprintf(stdout, "RAM not created!\n");
    return NULL;
  }
    return *(mem + addr);
}

