#include "memory.h"
#include <stdio.h>
#include <stdlib.h>

byte A = 0x00;
byte B = 0x00;
byte C = 0x00;
byte D = 0x00;
byte S = 0x00;
address IP = 0x0000;

byte* A_p = &A;
byte* B_p = &B;
byte* C_p = &C;
byte* D_p = &D;
byte* S_p = &S;
address* IP_p = &IP;

byte getop(memory RAM) {
  byte opcode = read(RAM, IP);
  fprintf(stdout, "byte 0x%02X at address 0x%04X\n", opcode, IP);
  (*IP_p)++;
  return opcode;
}

void mov_op(memory RAM, byte op) {
  switch (op) {
    case 0x01: {
                 byte data = getop(RAM);
                 fprintf(stdout, "MOVA 0x%02X\n", data);
                 (*A_p) = data;
                 if (A == 0x00) {
                   (*S_p) = S | 0x02;
                 }
                 break;
               }
  }
}

void init(memory RAM) {
  // Get the start address
  address LSB = read(RAM, 0xFFFE);
  address MSB = read(RAM, 0xFFFF);
  // shift the MSB 8 bits left so (--------)00000000 is MSB
  MSB = MSB << 8;
  // add the MSB (--------)00000000 and LSB 00000000(--------) to make (MSB)(LSB)
  (*IP_p) = MSB + LSB;
  while (1) {
    byte opcode = getop(RAM);
    if (opcode == 0x00) {
      fprintf(stdout, "NOP\n");
      continue;
    }
    if ((opcode > 0x00) & (opcode <= 0x05)) {
      mov_op(RAM, opcode);
      continue;
    }
  }
  return;
}
