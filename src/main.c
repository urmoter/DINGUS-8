#include <stdio.h>
#include "../headers/CPU.h"
#include "../headers/memory.h"

int main() {
    memory RAM = create_RAM();
    write(RAM, 0xFFFF, 0x01);
    write(RAM, 0x0100, 0x01);
    write(RAM, 0x0101, 0x0A);
    init(RAM);
    return 0;
}
