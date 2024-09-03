0x13: JMP (IMM16)ADR: Jump to ADR without saving IP on the stack.
0x14: CALL (IMM16)ADR: Jump to ADR but save IP on the stack.
0x15: RET: Pop the next 2 values (Should be IP) to IP
0x16: JZ (IMM16)ADR: Jump to ADR if Zero (Z) is set.
0x17: JNZ (IMM16)ADR: Jump to ADR if Zero (Z) is not set.