#include <stdio.h>
#include "../headers/CPU.h"
#include "../headers/memory.h"

int main() {
    memory RAM = create_RAM();
    readVROM(RAM, "ROM.bin");

    init(RAM);
    return 0;
}
