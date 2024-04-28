#include <stdio.h>
#include "memory.h"

int main() {
  memory RAM = create_RAM();
  fprintf(stdout, "0x%02X\n", read(RAM, 0x0000));
  write(RAM, 0x0000, 0x0A);
  fprintf(stdout, "0x%02X\n", read(RAM, 0x0000));
  return 0;
}
