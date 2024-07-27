#include <stdio.h>
#include "../headers/CPU.h"
#include "../headers/memory.h"

int main() {
    memory RAM = create_RAM();
    readVROM(RAM, "HelloWorld.bin");

    init(RAM);
    return 0;
}
