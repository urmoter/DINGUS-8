#include <stdio.h>
#include "memory.h"
#include "CPU.h"

int main() {
  memory RAM = create_RAM();
  write(RAM, 0xFFFF, 0x01);
  write(RAM, 0x0100, 0x01);
  write(RAM, 0x0101, 0x0A);
  init(RAM);
  return 0;
}
