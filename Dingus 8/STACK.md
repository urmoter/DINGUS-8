0x10: PSHI (IMM8):
	Set data at SP to IMM8.
	Decrement SP.
0x11: PSHR (REG):
	Set data at SP to the value of REG.
	Decrement SP.
0x12: POP (REG): [Z]
	Increment SP.
	Set REG’s value to the data at SP.
	Set the data at SP to 0x00.