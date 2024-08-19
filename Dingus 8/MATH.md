0x08: ADD (REG8)A (REG8)B: Add A to B and store in B [C, Z].
0x09: SUB (REG8)A (REG8)B: Subtract A from B and store in B [Z, N].
0x0A: INC (REG8): Increment REG by 1 [Z, C].
0x0B: DEC (REG8): Decrement REG by 1 [Z, N].
0x0C: ADC (REG8)A (REG8)B: Add A and (C) to B and store in B [Z, C].
0x0D: SBB (REG8)A (REG8)B: Subtract A and (N) from B [Z, N].
0x0E: NEG (REG8): Make REG Negative (2’s complement) [Z, N].
0x0F: FLP (REG8): Flip the register's value around (76543210) -> (01234567) [Z, N].