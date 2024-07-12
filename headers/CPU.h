#ifndef CPU
#define CPU

#include "memory.h"

byte getop(memory RAM);
int op_type(byte opcode);

void mov_op(memory RAM, byte op);
void math_op(memory RAM, byte op);
void stack_op(memory RAM, byte op);
void jump_op(memory RAM, byte op);
void halt_op();

void init(memory RAM);

#endif // !CPU
