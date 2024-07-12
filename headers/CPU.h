#ifndef CPU
#define CPU

#include "memory.h"

byte getop(memory RAM);
void mov_op(memory RAM, byte op);
void math_op(memory RAM, byte op) ;
void init(memory RAM);

#endif // !CPU
