#include <stdio.h>
#include "../headers/CPU.h"
#include "../headers/memory.h"

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        fprintf(stdout, "ONLY ONE RAM FILE!\n");
        return -1;
    }

    memory RAM = create_RAM();
    readVROM(RAM, (char*) argv[1]);

    init(RAM);
    return 0;
}
