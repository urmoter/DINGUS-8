```
 7 6 5 4 3 2 1 0
+-+-+-+-+-+-+-+-+
|H|-|T|C|P|N|Z|O|
+-+-+-+-+-+-+-+-+
	0x0: Overflow, set when a register exceeds 0xFF and wraps around.
	0x1: Zero, set if a register receives a 0x00 value.
	0x2: Negative, Overflow in Subtraction.
	0x3: Parity, 1 if odd, 0 if even number of bits.
	0x4: Carry, Overflow in Addition.
	0x5: Terminal, Active while printing to the Terminal memory address.
	0x6: Unused
	0x7: Halt, set when HLT (0x14) is called, clock will be ignored.
```