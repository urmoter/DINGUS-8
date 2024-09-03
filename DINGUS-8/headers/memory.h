#ifndef MEMORY
#define MEMORY

typedef unsigned char* memory;
typedef unsigned short address;
typedef unsigned char byte;

memory create_RAM();
void write(memory mem, address addr, byte data);
byte read(memory mem, address addr);
void readVROM(memory mem, char* ROMFILE);

#endif // !MEMORY
