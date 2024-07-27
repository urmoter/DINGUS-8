#include "../headers/memory.h"
#include <stdio.h>
#include <stdlib.h>

byte A = 0x00;
byte B = 0x00;
byte C = 0x00;
byte D = 0x00;
byte SP = 0xFF;
byte S = 0x00;
address IP = 0x0000;

byte *A_p = &A;
byte *B_p = &B;
byte *C_p = &C;
byte *D_p = &D;
byte *SP_p = &SP;
byte *S_p = &S;
address *IP_p = &IP;

// Making this fucking function made me want to blow my brains out.
byte flip_byte(byte data, FILE* log) {
    byte mask = 0x01;
    byte result = 0x00;
    int offset1 = 7;
    int offset2 = 1;

    // (7654_3210) -> (0123_4567)

    for (int i = 0; i < 8; i++) { // (____abcd) -> (dcba____)
        if (i < 4) {
            // Bit positions: 0->7, 1->6, 5->2, 3->4
            // Change by 7, 5, 3, 1
            // Decrement the change by 2
            result += ((data & mask) << offset1);
            mask <<= 1;
            offset1 -= 2;
        } else { // (abcd____) -> (____dcba)
            // TODO: FUCKING FIX THIS
            //       At this point, Mask should be 0001_0000.
            //       Keyword, S H O U L D.
            //       I am praying to god that it is.
            //       This half of the fucking function made me wish to die.
            //
            //       Potential Fix #1:
            //         Bit positions: 4->3, 5->2, 6->1, 7->0.
            //         Change by: 1, 3, 5, and 7.
            //         Increment the change by 2?
            //       What the fuck?
            //       That made it worse??
            //       Huh???
            //
            //       Potential Fix #2:
            //          Cock and ball torture?
            //
            //       Potential Fix #3:
            //          Kill myself??
            //      
            //       7/12/24, 2:07AM, I FUCKING DID IT, IT WORKS!
            //       OH YEAH, VECTOR.

            result += ((data & mask) >> offset2);
            mask <<= 1;
            offset2 += 2;
        }
    }
    return result;
}

byte getop(memory RAM, FILE* log) {
    byte opcode = read(RAM, IP);
    fprintf(log, "byte 0x%02X at address 0x%04X\n", opcode, IP);
    *(IP_p) = IP + 1;
    return opcode;
}

int op_type(byte opcode, FILE* log) {
    if (opcode == 0x00) {
        fprintf(log, "NOP\n");
        return -1;
    }
    if ((opcode > 0x00) && (opcode < 0x06)) {
        return 0;
    }
    if ((opcode > 0x05) && (opcode < 0x08)) {
        return 1;
    }
    if ((opcode > 0x07) && (opcode < 0x10)) {
        return 2;
    }
    if ((opcode > 0x0A) && (opcode < 0x13)) {
        return 3;
    }
    if ((opcode > 0x12) && (opcode < 0x18)) {
        return 4;
    }
    if (opcode == 0x18) {
        return 5;
    }
    if ((opcode == 0x19) || (opcode == 0x1A)) {
        return 6;
    }
    if ((opcode > 0x1A) && (opcode < 0x25)) {
        return 7;
    }
    if ((opcode > 0x24) && (opcode < 0x29)) {
        return 8;
    }
    if ((opcode == 0x29) || (opcode == 0x2A)) {
        return 9;
    }
    if ((opcode == 0x2B) || (opcode == 0x2C)) {
        return 10;
    }
    if ((opcode > 0x2C) && (opcode < 0x30)) {
        return 11;
    }

    fprintf(log, "Not a valid opcode!! (0x%02X)\n", opcode);
    return -1;
}

void mov_op(memory RAM, byte op, FILE* log) {
    switch (op) {
        case 0x01: {
            byte data = getop(RAM, log);
            fprintf(log, "MOVA $%02X\n", data);
            (*A_p) = data;
            if (A == 0x00) {
                (*S_p) = S | 0x02;
            }
            break;
        }
        case 0x02: {
            byte data = getop(RAM, log);
            fprintf(log, "MOVB $%02X\n", data);
            (*B_p) = data;
            if (B == 0x00) {
                (*S_p) = S | 0x02;
            }
            break;
        }
        case 0x03: {
            byte data = getop(RAM, log);
            fprintf(log, "MOVA 0x%02X\n", data);
            (*C_p) = data;
            if (C == 0x00) {
                (*S_p) = S | 0x02;
            }
            break;
        }
        case 0x04: {
            byte data = getop(RAM, log);
            fprintf(log, "MOVA 0x%02X\n", data);
            (*D_p) = data;
            if (D == 0x00) {
                (*S_p) = S | 0x02;
            }
            break;
        }
        case 0x05: {
            byte RegA = getop(RAM, log);
            byte RegB = getop(RAM, log);
            // Here we go!
            // Switchception!!
            switch (RegA) {
                case 0x00: {
                    // A source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "CPY %%A, %%A\n");
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "CPY %%A, %%B\n");
                            (*B_p) = A;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "CPY %%A, %%C\n");
                            (*C_p) = A;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "CPY %%A, %%D\n");
                            (*D_p) = A;
                            if (D == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }   
                    }
                    break;
                }
                case 0x01: {
                    // B source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "CPY %%B, %%A\n");
                            (*A_p) = B;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "CPY %%B, %%B\n");
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "CPY %%B, %%C\n");
                            (*C_p) = B;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "CPY %%B, %%D\n");
                            (*D_p) = B;
                            if (D == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x02: {
                    // C source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "CPY %%C, %%A\n");
                            (*A_p) = C;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "CPY %%C, %%B\n");
                            (*B_p) = C;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "CPY %%C, %%C\n");
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "CPY %%C, %%D\n");
                            (*D_p) = C;
                            if (D == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x03: {
                    // D source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "CPY %%D, %%A\n");
                            (*A_p) = D;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "CPY %%D, %%B\n");
                            (*B_p) = D;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "CPY %%D, %%C\n");
                            (*C_p) = D;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "CPY %%D, %%D\n");
                            if (D == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                default: {
                    fprintf(log, "Invalid A register, 0x%02X!\n", RegA);
                    break;
                }
            }
            break;
        }
    }
}
void mem_op(memory RAM, byte op, FILE* log) {
    switch (op) {
        case 0x06: {
            // Source Register
            byte Reg = getop(RAM, log);
            // Halves of the address
            byte LSB = getop(RAM, log);
            address MSB = getop(RAM, log);
             // shift the MSB 8 bits left so (--------)00000000 is MSB
            MSB = MSB << 8;
            // add the MSB (--------)00000000 and LSB 00000000(--------) to make (MSB)(LSB)
            address addr = MSB + LSB;

            switch (Reg) {
                case 0x00: {
                    fprintf(log, "STR %%A, @%04X\n", addr);
                    write(RAM, addr, A);
                    if (A == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }
                case 0x01: {
                    fprintf(log, "STR %%B, @%04X\n", addr);
                    write(RAM, addr, B);
                    if (B == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }
                case 0x02: {
                    fprintf(log, "STR %%C, @%04X\n", addr);
                    write(RAM, addr, C);
                    if (C == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }
                case 0x03: {
                    fprintf(log, "STR %%D, @%04X\n", addr);
                    write(RAM, addr, D);
                    if (D == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }

                default: {
                    fprintf(log, "Invalid register, 0x%02X!\n", Reg);
                    break;
                }
            }
            break;
        }

        case 0x07: {
            // Halves of the address
            byte LSB = getop(RAM, log);
            address MSB = getop(RAM, log);
             // shift the MSB 8 bits left so (--------)00000000 is MSB
            MSB = MSB << 8;
            // add the MSB (--------)00000000 and LSB 00000000(--------) to make (MSB)(LSB)
            address addr = MSB + LSB;
            // Destination Register
            byte Reg = getop(RAM, log);

            switch (Reg) {
                case 0x00: {
                    fprintf(log, "LDR @%04X, %%A\n", addr);
                    (*A_p) = read(RAM, addr);
                    if (A == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }
                case 0x01: {
                    fprintf(log, "LDR @%04X, %%B\n", addr);
                    (*B_p) = read(RAM, addr);
                    if (B == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }
                case 0x02: {
                    fprintf(log, "LDR @%04X, %%C\n", addr);
                    (*C_p) = read(RAM, addr);
                    if (C == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }
                case 0x03: {
                    fprintf(log, "LDR @%04X, %%D\n", addr);
                    (*D_p) = read(RAM, addr);
                    if (D == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }

                default: {
                    fprintf(log, "Invalid register, 0x%02X!\n", Reg);
                    break;
                }
            }
        }
    }
}
void math_op(memory RAM, byte op, FILE* log) {
    switch (op) {
        case 0x08: {
            byte RegA = getop(RAM, log);
            byte RegB = getop(RAM, log);
            // Here we go!
            // Switchception!!
            switch (RegA) {
                case 0x00: {
                    // A source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "ADD %%A, %%A\n");
                            if (( A+A) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*A_p) = A + A;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "ADD %%A, %%B\n");
                            if (( A+B) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*B_p) = A + B;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "ADD %%A, %%C\n");
                            if (( A+C) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*C_p) = A + C;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "ADD %%A, %%D\n");
                            if (( A+D) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*D_p) = A + D;
                            if (D == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x01: {
                    // B source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "ADD %%B, %%A\n");
                            if (( B+A) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*A_p) = B + A;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "ADD %%B, %%B\n");
                            if (( B+B) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*B_p) = B + B;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "ADD %%B, %%C\n");
                            if (( B+C) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*C_p) = B + C;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "ADD %%B, %%D\n");
                            if (( B+D) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*D_p) = B + D;
                            if (D == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x02: {
                    // C source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "ADD %%C, %%A\n");
                            if (( C+A) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*A_p) = C + A;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "ADD %%C, %%B\n");
                            if (( C+B) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*B_p) = C + B;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "ADD %%C, %%C\n");
                            if (( C+C) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*C_p) = C + C;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "ADD %%C, %%D\n");
                            if (( C+D) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*D_p) = C + D;
                            if (D == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x03: {
                    // D source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "ADD %%D, %%A\n");
                            if (( D+A) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*A_p) = D + A;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "ADD %%D, %%B\n");
                            if (( D+B) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*B_p) = D + B;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "ADD %%D, %%C\n");
                            if (( D+C) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*C_p) = D + C;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "ADD %%D, %%D\n");
                            if (( D+D) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*D_p) = D + D;
                            if (D == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                }
                default: {
                    fprintf(log, "Invalid A register, 0x%02X!\n", RegA);
                    break;
                }
            }
            break;
        }
        case 0x09: {
            byte RegA = getop(RAM, log);
            byte RegB = getop(RAM, log);
            // Here we go!
            // Switchception!!
            switch (RegA) {
                case 0x00: {
                    // A source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "SUB %%A, %%A\n");
                            if (( A-A) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*A_p) = A - A;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "SUB %%A, %%B\n");
                            if (( A-B) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*B_p) = A - B;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "SUB %%A, %%C\n");
                            if (( A-C) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*C_p) = A - C;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "SUB %%A, %%D\n");
                            if (( A-D) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*D_p) = A - D;
                            if (D == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x01: {
                    // B source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "SUB %%B, %%A\n");
                            if (( B-A) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*A_p) = B - A;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "SUB %%B, %%B\n");
                            if (( B-B) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*B_p) = B - B;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "SUB %%B, %%C\n");
                            if (( B-C) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*C_p) = B - C;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "SUB %%B, %%D\n");
                            if (( B-D) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*D_p) = B - D;
                            if (D == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x02: {
                    // C source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "SUB %%C, %%A\n");
                            if (( C-A) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*A_p) = C - A;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "SUB %%C, %%B\n");
                            if (( C-B) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*B_p) = C - B;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "SUB %%C, %%C\n");
                            if (( C-C) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*C_p) = C - C;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "SUB %%C, %%D\n");
                            if (( C-D) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*D_p) = C - D;
                            if (D == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x03: {
                    // D source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "SUB %%D, %%A\n");
                            if (( D-A) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*A_p) = D - A;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "SUB %%D, %%B\n");
                            if (( D-B) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*B_p) = D - B;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "SUB %%D, %%C\n");
                            if (( D-C) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*C_p) = D - C;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "SUB %%D, %%D\n");
                            if (( D-D) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*D_p) = D - D;
                            if (D == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                default: {
                    fprintf(log, "Invalid A register, 0x%02X!\n", RegA);
                    break;
                }
            }
            break;
        }
        case 0x0A: {
            byte reg = getop(RAM, log);
            switch (reg) {
                case 0x00: {
                    fprintf(log, "INC %%A\n");
                    if (A == 0xFF) {
                        (*S_p) = S | 0x12;
                    }
                    (*A_p) = A + 1;
                    break;
                }
                case 0x01: {
                    fprintf(log, "INC %%B\n");
                    if (B == 0xFF) {
                        (*S_p) = S | 0x12;
                    }
                    (*B_p) = B + 1;
                    break;
                }
                case 0x02: {
                    fprintf(log, "INC %%C\n");
                    if (C == 0xFF) {
                        (*S_p) = S | 0x12;
                    }
                    (*C_p) = C + 1;
                    break;
                }
                case 0x03: {
                    fprintf(log, "INC %%D\n");
                    if (D == 0xFF) {
                        (*S_p) = S | 0x12;
                    }
                    (*D_p) = D + 1;
                    break;
                }
            }
            break;
        }
        case 0x0B: {
            byte reg = getop(RAM, log);
            switch (reg) {
                fprintf(log, "DEC %%A\n");
                case 0x00: {
                    if (A == 0x00) {
                        (*S_p) = S | 0x10;
                    }
                    (*A_p) = A - 1;
                    if (A == 0x00) {
                        (*S_p) = S | 0x04;
                    }
                    break;
                }
                case 0x01: {
                    fprintf(log, "DEC %%B\n");
                    if (B == 0x00) {
                        (*S_p) = S | 0x10;
                    }
                    (*B_p) = B - 1;
                    if (B == 0x00) {
                        (*S_p) = S | 0x04;
                    }
                    break;
                }
                case 0x02: {
                    fprintf(log, "DEC %%C\n");
                    if (C == 0x00) {
                        (*S_p) = S | 0x10;
                    }
                    (*C_p) = C - 1;
                    if (C == 0x00) {
                        (*S_p) = S | 0x04;
                    }
                    break;
                }
                case 0x03: {
                    fprintf(log, "DEC %%D\n");
                    if (D == 0x00) {
                        (*S_p) = S | 0x10;
                    }
                    (*D_p) = D - 1;
                    if (D == 0x00) {
                        (*S_p) = S | 0x04;
                    }
                    break;
                }
            }
            break;
        }
        case 0x0C: {
            byte carry = ((S & 0x10) == 0x10) ? 0x01 : 0x00;
            byte RegA = getop(RAM, log);
            byte RegB = getop(RAM, log);
            // Here we go!
            // Switchception!!
            switch (RegA) {
                case 0x00: {
                    // A source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "ADC %%A, %%A\n");
                            if (( A+A+carry) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*A_p) = A + A + carry;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "ADC %%A, %%B\n");
                            if (( A+B+carry) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*B_p) = A + B + carry;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "ADC %%A, %%C\n");
                            if (( A+C+carry) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*C_p) = A + C + carry;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "ADC %%A, %%D\n");
                            if (( A+D+carry) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*D_p) = A + D + carry;
                            if (D == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x01: {
                    // B source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "ADC %%B, %%A\n");
                            if (( B+A+carry) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*A_p) = B + A + carry;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "ADC %%B, %%B\n");
                            if (( B+B+carry) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*B_p) = B + B + carry;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "ADC %%B, %%C\n");
                            if (( B+C+carry) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*C_p) = B + C + carry;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "ADC %%B, %%D\n");
                            if (( B+D+carry) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*D_p) = B + D + carry;
                            if (D == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x02: {
                    // C source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "ADC %%C, %%A\n");
                            if (( C+A+carry) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*A_p) = C + A + carry;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "ADC %%C, %%B\n");
                            if (( C+B+carry) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*B_p) = C + B + carry;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "ADC %%C, %%C\n");
                            if (( C+C+carry) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*C_p) = C + C + carry;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "ADC %%C, %%D\n");
                            if (( C+D+carry) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*D_p) = C + D + carry;
                            if (D == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x03: {
                    // D source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "ADC %%D, %%A\n");
                            if (( D+A+carry) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*A_p) = D + A + carry;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "ADC %%D, %%B\n");
                            if (( D+B+carry) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*B_p) = D + B + carry;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "ADC %%D, %%C\n");
                            if (( D+C+carry) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*C_p) = D + C + carry;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "ADC %%D, %%D\n");
                            if (( D+D+carry) > 0xFF) {
                                (*S_p) = S | 0x10;
                            }
                            (*D_p) = D + D + carry;
                            if (D == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                default: {
                    fprintf(log, "Invalid A register, 0x%02X!\n", RegA);
                    break;
                }
            }
            break;
        }
        case 0x0D: {
            byte borrow = ((S & 0x04) == 0x04) ? 0x01 : 0x00;
            byte RegA = getop(RAM, log);
            byte RegB = getop(RAM, log);
            // Here we go!
            // Switchception!!
            switch (RegA) {
                case 0x00: {
                    // A source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "SBB %%A, %%A\n");
                            if (( A-A-borrow) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*A_p) = A - A - borrow;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "SBB %%A, %%B\n");
                            if (( A-B-borrow) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*B_p) = A - B - borrow;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "SBB %%A, %%C\n");
                            if (( A-C-borrow) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*C_p) = A - C - borrow;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "SBB %%A, %%D\n");
                            if (( A-D-borrow) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*D_p) = A - D - borrow;
                            if (D == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x01: {
                    // B source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "SBB %%B, %%A\n");
                            if (( B-A-borrow) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*A_p) = B - A - borrow;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "SBB %%B, %%B\n");
                            if (( B-B-borrow) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*B_p) = B - B - borrow;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "SBB %%B, %%C\n");
                            if (( B-C-borrow) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*C_p) = B - C - borrow;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "SBB %%B, %%D\n");
                            if (( B-D-borrow) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*D_p) = B - D - borrow;
                            if (D == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x02: {
                    // C source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "SBB %%C, %%A\n");
                            if (( C-A-borrow) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*A_p) = C - A - borrow;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "SBB %%C, %%B\n");
                            if (( C-B-borrow) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*B_p) = C - B - borrow;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "SBB %%C, %%C\n");
                            if (( C-C-borrow) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*C_p) = C - C - borrow;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "SBB %%C, %%D\n");
                            if (( C-D-borrow) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*D_p) = C - D - borrow;
                            if (D == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x03: {
                    // D source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "SBB %%D, %%A\n");
                            if (( D-A-borrow) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*A_p) = D - A - borrow;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "SBB %%D, %%B\n");
                            if (( D-B-borrow) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*B_p) = D - B - borrow;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "SBB %%D, %%C\n");
                            if (( D-C-borrow) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*C_p) = D - C - borrow;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "SBB %%D, %%D\n");
                            if (( D-D-borrow) >= 0x80) {
                                (*S_p) = S | 0x04;
                            }
                            (*D_p) = D - D - borrow;
                            if (D == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                default: {
                    fprintf(log, "Invalid A register, 0x%02X!\n", RegA);
                    break;
                }
            }
            break;
        }
        case 0x0E: {
            byte reg = getop(RAM, log);
            switch (reg) {
                case 0x00: {
                    fprintf(log, "NEG %%A\n");
                    (*A_p) = (~A)+1;
                    if (A >= 0x80) {
                        (*S_p) = S | 0x04;
                    }
                    if (A == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }

                case 0x01: {
                    fprintf(log, "NEG %%B\n");
                    (*B_p) = (~B)+1;
                    if (B >= 0x80) {
                        (*S_p) = S | 0x04;
                    }
                    if (B == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }

                case 0x02: {
                    fprintf(log, "NEG %%C\n");
                    (*C_p) = (~C)+1;
                    if (C >= 0x80) {
                        (*S_p) = S | 0x04;
                    }
                    if (C == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }

                case 0x03: {
                    fprintf(log, "NEG %%D\n");
                    (*D_p) = (~D)+1;
                    if (D >= 0x80) {
                        (*S_p) = S | 0x04;
                    }
                    if (D == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }

                default: {
                    fprintf(log, "0x%02X is an invalid register!\n", reg);
                    break;
                }
            }
            break;
        }
        case 0x0F: {
            byte reg = getop(RAM, log);
            switch (reg) {
                case 0x00: {
                    fprintf(log, "FLP %%A\n");
                    (*A_p) = flip_byte(A, log);
                    if (A >= 0x80) {
                        (*S_p) = S | 0x04;
                    }
                    if (A == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }

                case 0x01: {
                    fprintf(log, "FLP %%B\n");
                    (*B_p) = flip_byte(B, log);
                    if (B >= 0x80) {
                        (*S_p) = S | 0x04;
                    }
                    if (B == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }

                case 0x02: {
                    fprintf(log, "FLP %%C\n");
                    (*C_p) = flip_byte(C, log);
                    if (C >= 0x80) {
                        (*S_p) = S | 0x04;
                    }
                    if (C == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }

                case 0x03: {
                    fprintf(log, "FLP %%D\n");
                    (*D_p) = flip_byte(D, log);
                    if (D >= 0x80) {
                        (*S_p) = S | 0x04;
                    }
                    if (D == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }

                default: {
                    fprintf(log, "0x%02X is an invalid register!\n", reg);
                    break;
                }
            }
            break;
        }
    }
}
void stack_op(memory RAM, byte op, FILE* log) {
    switch (op) {
        case 0x10: {
            byte data = getop(RAM, log);
            write(RAM, (address) SP, data);
            *(SP_p) = SP - 1;
            fprintf(log, "PSHI $%02X\n", data);
            break;
        }

        case 0x11: {
            byte reg = getop(RAM, log);
            switch (reg) {
                case 0x00: {
                    write(RAM, (address) SP, A);
                    *(SP_p) = SP - 1;
                    fprintf(log, "PSHR %%A\n");
                    break;
                }

                case 0x01: {
                    write(RAM, (address) SP, B);
                    *(SP_p) = SP - 1;
                    fprintf(log, "PSHR %%B\n");
                    break;
                }

                case 0x02: {
                    write(RAM, (address) SP, C);
                    *(SP_p) = SP - 1;
                    fprintf(log, "PSHR %%C\n");
                    break;
                }

                case 0x03: {
                    write(RAM, (address) SP, D);
                    *(SP_p) = SP - 1;
                    fprintf(log, "PSHR %%D\n");
                    break;
                }

                default: {
                    fprintf(log, "Invalid register, 0x%02X!\n", reg);
                    break;
                }
            }

            break;
        }

        case 0x12: {
            byte reg = getop(RAM, log);
            switch (reg) {
                case 0x00: {
                    *(SP_p) = SP + 1;
                    *(A_p) = read(RAM, SP);
                    write(RAM, SP, 0x00);
                    fprintf(log, "POP %%A\n");
                    break;
                }

                case 0x01: {
                    *(SP_p) = SP + 1;
                    *(B_p) = read(RAM, SP);
                    write(RAM, SP, 0x00);
                    fprintf(log, "POP %%B\n");
                    break;
                }

                case 0x02: {
                    *(SP_p) = SP + 1;
                    *(C_p) = read(RAM, SP);
                    write(RAM, SP, 0x00);
                    fprintf(log, "POP %%C\n");
                    break;
                }

                case 0x03: {
                    *(SP_p) = SP + 1;
                    *(D_p) = read(RAM, SP);
                    write(RAM, SP, 0x00);
                    fprintf(log, "POP %%D\n");
                    break;
                }

                default: {
                    fprintf(log, "Invalid register, 0x%02X!\n", reg);
                    break;
                }
            }

            break;
        }
    }
}
void jump_op(memory RAM, byte op, FILE* log) {
    switch (op) {
        case 0x13: {
            // Halves of the address
            byte LSB = getop(RAM, log);
            address MSB = getop(RAM, log);
             // shift the MSB 8 bits left so (--------)00000000 is MSB
            MSB = MSB << 8;
            // add the MSB (--------)00000000 and LSB 00000000(--------) to make (MSB)(LSB)
            address addr = MSB + LSB;
            *(IP_p) = addr;
            fprintf(log, "JMP @%04X\n", addr);
            break;
        }

        case 0x14: {
            // Halves of the address
            byte LSB = getop(RAM, log);
            address MSB = getop(RAM, log);
             // shift the MSB 8 bits left so (--------)00000000 is MSB
            MSB = MSB << 8;
            // add the MSB (--------)00000000 and LSB 00000000(--------) to make (MSB)(LSB)
            address addr = MSB + LSB;
            // Saving the IP
            write(RAM, (address) SP, IP);
            *(SP_p) = SP - 1;
            write(RAM, (address) SP, (IP >> 4));
            *(SP_p) = SP - 1;
            // Jumping to the addr
            *(IP_p) = addr;
            fprintf(log, "CALL @%04X\n", addr);
            break;
        }

        case 0x15: {
            // Popping  IP
            *(SP_p) = SP + 1;
            address MSB = read(RAM, SP);
            write(RAM, SP, 0x00);

            *(SP_p) = SP + 1;
            byte LSB = read(RAM, SP);
            write(RAM, SP, 0x00);

            MSB = MSB << 8;
            
            *(IP_p) = MSB;
            fprintf(log, "RET\n");
        }
    
        case 0x16: {
            // Halves of the address
            byte LSB = getop(RAM, log);
            address MSB = getop(RAM, log);
            // shift the MSB 8 bits left so (--------)00000000 is MSB
            MSB = MSB << 8;
            // add the MSB (--------)00000000 and LSB 00000000(--------) to make (MSB)(LSB)
            address addr = MSB + LSB;
            if ((S & 0x02) == 0) {
                *(IP_p) = addr;
                fprintf(log, "JZ @%04X\n", addr);
                break;
            } else {
                fprintf(log, "JZ @%04X\n", addr);
                break;
            }
        }

        case 0x17: {
            // Halves of the address
            byte LSB = getop(RAM, log);
            address MSB = getop(RAM, log);
            // shift the MSB 8 bits left so (--------)00000000 is MSB
            MSB = MSB << 8;
            // add the MSB (--------)00000000 and LSB 00000000(--------) to make (MSB)(LSB)
            address addr = MSB + LSB;
            if ((S & 0x02) != 0) {
                *(IP_p) = addr;
                fprintf(log, "JNZ @%04X\n", addr);
                break;
            } else {
                fprintf(log, "JNZ @%04X\n", addr);
                break;
            }
        }
    }
}
void halt_op(FILE* log) {
    fprintf(log, "General Purpose Registers:\nA: %02X\nB: %02X\nC: %02X\nD: %02X\n", A, B, C, D);
    fprintf(log, "Special Registers:\nS: %02X\nSP: %04X\nIP: %04X\n", S, SP, IP);

    *(S_p) = S | 0x80;
    return;
}
void over_op(memory RAM, byte op, FILE* log) {
    switch (op) {
        case 0x19: {
            // Halves of the address
            byte LSB = getop(RAM, log);
            address MSB = getop(RAM, log);
            // shift the MSB 8 bits left so (--------)00000000 is MSB
            MSB = MSB << 8;
            // add the MSB (--------)00000000 and LSB 00000000(--------) to make (MSB)(LSB)
            address addr = MSB + LSB;
            if ((S & 0x01) == 0) {
                *(IP_p) = addr;
                fprintf(log, "JO @%04X\n", addr);
                break;
            } else {
                fprintf(log, "JO @%04X\n", addr);
                break;
            }
        }

        case 0x1A: {
            // Halves of the address
            byte LSB = getop(RAM, log);
            address MSB = getop(RAM, log);
            // shift the MSB 8 bits left so (--------)00000000 is MSB
            MSB = MSB << 8;
            // add the MSB (--------)00000000 and LSB 00000000(--------) to make (MSB)(LSB)
            address addr = MSB + LSB;
            if ((S & 0x01) != 0) {
                *(IP_p) = addr;
                fprintf(log, "JNO @%04X\n", addr);
                break;
            } else {
                fprintf(log, "JNO @%04X\n", addr);
                break;
            }
        }
    }
}
void status_op(memory RAM, byte op, FILE* log) {
    switch (op) {
        // Overflow
        case 0x1B: {
            *(S_p) = S | 0x01;
            fprintf(log, "SETO\n");
            break;
        }

        case 0x1C: {
            *(S_p) = S & 0xFE;
            fprintf(log, "CLRO\n");
            break;
        }

        // Zero
        case 0x1D: {
            *(S_p) = S | 0x02;
            fprintf(log, "SETZ\n");
            break;
        }

        case 0x1E: {
            *(S_p) = S & 0xF;
            fprintf(log, "CLRZ\n");
            break;
        }

        // Negative
        case 0x1F: {
            *(S_p) = S | 0x04;
            fprintf(log, "SETN\n");
            break;
        }

        case 0x20: {
            *(S_p) = S & 0xFB;
            fprintf(log, "CLRN\n");
            break;
        }

        // Parity
        case 0x21: {
            *(S_p) = S | 0x08;
            fprintf(log, "SETP\n");
            break;
        }

        case 0x22: {
            *(S_p) = S & 0xF7;
            fprintf(log, "CLRP\n");
            break;
        }

        // Carry
        case 0x23: {
            *(S_p) = S | 0x10;
            fprintf(log, "SETC\n");
            break;
        }

        case 0x24: {
            *(S_p) = S & 0xEF;
            fprintf(log, "CLRC\n");
            break;
        }
    }
}
void logic_op(memory RAM, byte op, FILE* log) {
    switch (op) {
        case 0x25: {
            byte RegA = getop(RAM, log);
            byte RegB = getop(RAM, log);
            // Here we go!
            // Switchception!!
            switch (RegA) {
                case 0x00: {
                    // A source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "AND %%A, %%A");
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "AND %%A, %%B");
                            *(A_p) = A & B;
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "AND %%A, %%C");
                            *(A_p) = A & C;
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "AND %%A, %%D");
                            *(A_p) = A & D;
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                }
                case 0x01: {
                    // B source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "AND %%B, %%A");
                            *(B_p) = B & A;
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "AND %%B, %%B");\
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "AND %%B, %%C");
                            *(B_p) = B & C;
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "AND %%B, %%D");
                            *(B_p) = B & D;
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                }
                case 0x02: {
                    // C source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "AND %%C, %%A");
                            *(C_p) = C & A;
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "AND %%C, %%B");
                            *(C_p) = C & B;
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "AND %%C, %%C");
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "AND %%C, %%D");
                            *(C_p) = C & D;
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                }
                case 0x03: {
                    // D source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "AND %%D, %%A");
                            *(D_p) = D & A;
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "AND %%D, %%B");
                            *(D_p) = D & B;
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "AND %%D, %%C");
                            *(D_p) = D & C;
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "AND %%D, %%D");
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                }
                default: {
                    fprintf(log, "Invalid A register, 0x%02X!\n", RegA);
                    break;
                }
            }
            break;
        }

        case 0x26: {
            byte RegA = getop(RAM, log);
            byte RegB = getop(RAM, log);
            // Here we go!
            // Switchception!!
            switch (RegA) {
                case 0x00: {
                    // A source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "OR %%A, %%A");
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "OR %%A, %%B");
                            *(A_p) = A | B;
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "OR %%A, %%C");
                            *(A_p) = A | C;
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "OR %%A, %%D");
                            *(A_p) = A | D;
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                }
                case 0x01: {
                    // B source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "OR %%B, %%A");
                            *(B_p) = B | A;
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "OR %%B, %%B");\
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "OR %%B, %%C");
                            *(B_p) = B | C;
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "OR %%B, %%D");
                            *(B_p) = B | D;
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                }
                case 0x02: {
                    // C source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "OR %%C, %%A");
                            *(C_p) = C | A;
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "OR %%C, %%B");
                            *(C_p) = C | B;
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "OR %%C, %%C");
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "OR %%C, %%D");
                            *(C_p) = C | D;
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                }
                case 0x03: {
                    // D source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "OR %%D, %%A");
                            *(D_p) = D | A;
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "OR %%D, %%B");
                            *(D_p) = D | B;
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "OR %%D, %%C");
                            *(D_p) = D | C;
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "OR %%D, %%D");
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                }
                default: {
                    fprintf(log, "Invalid A register, 0x%02X!\n", RegA);
                    break;
                }
            }
            break;
        }

        case 0x27: {
            byte RegA = getop(RAM, log);
            byte RegB = getop(RAM, log);
            // Here we go!
            // Switchception!!
            switch (RegA) {
                case 0x00: {
                    // A source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "XOR %%A, %%A");
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "XOR %%A, %%B");
                            *(A_p) = A ^ B;
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "XOR %%A, %%C");
                            *(A_p) = A ^ C;
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "XOR %%A, %%D");
                            *(A_p) = A ^ D;
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                }
                case 0x01: {
                    // B source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "XOR %%B, %%A");
                            *(B_p) = B ^ A;
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "XOR %%B, %%B");\
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "XOR %%B, %%C");
                            *(B_p) = B ^ C;
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "XOR %%B, %%D");
                            *(B_p) = B ^ D;
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                }
                case 0x02: {
                    // C source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "XOR %%C, %%A");
                            *(C_p) = C ^ A;
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "XOR %%C, %%B");
                            *(C_p) = C ^ B;
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "XOR %%C, %%C");
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "XOR %%C, %%D");
                            *(C_p) = C ^ D;
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                }
                case 0x03: {
                    // D source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "XOR %%D, %%A");
                            *(D_p) = D ^ A;
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "XOR %%D, %%B");
                            *(D_p) = D ^ B;
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "XOR %%D, %%C");
                            *(D_p) = D ^ C;
                            break;
                        }
                        case 0x03: {
                            fprintf(log, "XOR %%D, %%D");
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                }
                default: {
                    fprintf(log, "Invalid A register, 0x%02X!\n", RegA);
                    break;
                }
            }
            break;
        }
    
        case 0x28: {
            byte Reg = getop(RAM, log);

            switch (Reg) {
                case 0x00: {
                    fprintf(log, "NOT %%A\n");
                    *(A_p) = ~A;
                    break;
                }

                case 0x01: {
                    fprintf(log, "NOT %%B\n");
                    *(B_p) = ~B;
                    break;
                }

                case 0x02: {
                    fprintf(log, "NOT %%C\n");
                    *(C_p) = ~C;
                    break;
                }

                case 0x03: {
                    fprintf(log, "NOT %%D\n");
                    *(D_p) = ~D;
                    break;
                }

                default: {
                    fprintf(log, "Invalid register, 0x%02X!\n", Reg);
                    break;
                }
            }
        }
    }
}
void equal_op(memory RAM, byte op, FILE* log) {
    switch (op) {
        case 0x29: {
            // Halves of the address
            byte LSB = getop(RAM, log);
            address MSB = getop(RAM, log);
             // shift the MSB 8 bits left so (--------)00000000 is MSB
            MSB = MSB << 8;
            // add the MSB (--------)00000000 and LSB 00000000(--------) to make (MSB)(LSB)
            address addr = MSB + LSB;

            byte RegA = getop(RAM, log);
            byte RegB = getop(RAM, log);

            switch (RegA) {
                case 0x00: {
                    // A source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "JE @%04X, %%A, %%A\n", addr);
                            *(IP_p) = addr;
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "JE @%04X, %%A, %%B\n", addr);
                            if (A == B) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "JE @%04X, %%A, %%C\n", addr);
                            if (A == C) {
                                *(IP_p) = addr;
                            }
                        }
                        case 0x03: {
                            fprintf(log, "JE @%04X, %%A, %%D\n", addr);
                            if (A == D) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }   
                    }
                    break;
                }
                case 0x01: {
                    // B source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "JE @%04X, %%B, %%A\n", addr);
                            if (B == A) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "JE @%04X, %%B, %%B\n", addr);
                            *(IP_p) = addr;
                            
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "JE @%04X, %%B, %%C\n", addr);
                            if (B == C) {
                                *(IP_p) = addr;
                            }
                        }
                        case 0x03: {
                            fprintf(log, "JE @%04X, %%B, %%D\n", addr);
                            if (B == D) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x02: {
                    // C source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "JE @%04X, %%C, %%A\n", addr);
                            if (C == A) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "JE @%04X, %%C, %%B\n", addr);
                            if (C == B) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "JE @%04X, %%C, %%C\n", addr);
                            *(IP_p) = addr;
                        }
                        case 0x03: {
                            fprintf(log, "JE @%04X, %%C, %%D\n", addr);
                            if (C == D) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x03: {
                    // D source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "JE @%04X, %%D, %%A\n", addr);
                            if (D == A) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "JE @%04X, %%D, %%B\n", addr);
                            if (D == B) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "JE @%04X, %%D, %%C\n", addr);
                            if (D == C) {
                                *(IP_p) = addr;
                            }
                        }
                        case 0x03: {
                            fprintf(log, "JE @%04X, %%D, %%D\n", addr);
                            *(IP_p) = addr;
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }   
                    }
                }
                default: {
                    fprintf(log, "Invalid A register, 0x%02X!\n", RegA);
                    break;
                }
            }
        }
        
        case 0x2A: {
            // Halves of the address
            byte LSB = getop(RAM, log);
            address MSB = getop(RAM, log);
             // shift the MSB 8 bits left so (--------)00000000 is MSB
            MSB = MSB << 8;
            // add the MSB (--------)00000000 and LSB 00000000(--------) to make (MSB)(LSB)
            address addr = MSB + LSB;

            byte RegA = getop(RAM, log);
            byte RegB = getop(RAM, log);

            switch (RegA) {
                case 0x00: {
                    // A source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "JNE @%04X, %%A, %%A\n", addr);
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "JNE @%04X, %%A, %%B\n", addr);
                            if (A != B) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "JNE @%04X, %%A, %%C\n", addr);
                            if (A != C) {
                                *(IP_p) = addr;
                            }
                        }
                        case 0x03: {
                            fprintf(log, "JNE @%04X, %%A, %%D\n", addr);
                            if (A != D) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }   
                    }
                    break;
                }
                case 0x01: {
                    // B source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "JNE @%04X, %%B, %%A\n", addr);
                            if (B != A) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "JNE @%04X, %%B, %%B\n", addr);
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "JNE @%04X, %%B, %%C\n", addr);
                            if (B != C) {
                                *(IP_p) = addr;
                            }
                        }
                        case 0x03: {
                            fprintf(log, "JE @%04X, %%B, %%D\n", addr);
                            if (B != D) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x02: {
                    // C source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "JNE @%04X, %%C, %%A\n", addr);
                            if (C != A) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "JNE @%04X, %%C, %%B\n", addr);
                            if (C != B) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "JNE @%04X, %%C, %%C\n", addr);
                        }
                        case 0x03: {
                            fprintf(log, "JNE @%04X, %%C, %%D\n", addr);
                            if (C != D) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x03: {
                    // D source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "JNE @%04X, %%D, %%A\n", addr);
                            if (D != A) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "JNE @%04X, %%D, %%B\n", addr);
                            if (D != B) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "JNE @%04X, %%D, %%C\n", addr);
                            if (D != C) {
                                *(IP_p) = addr;
                            }
                        }
                        case 0x03: {
                            fprintf(log, "JNE @%04X, %%D, %%D\n", addr);
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }   
                    }
                }
                default: {
                    fprintf(log, "Invalid A register, 0x%02X!\n", RegA);
                    break;
                }
            }
        }
    }
}
void ineq_op(memory RAM, byte op, FILE* log) {
    switch (op) {
        case 0x2B: {
            // Halves of the address
            byte LSB = getop(RAM, log);
            address MSB = getop(RAM, log);
            // shift the MSB 8 bits left so (--------)00000000 is MSB
            MSB = MSB << 8;
            // add the MSB (--------)00000000 and LSB 00000000(--------) to make (MSB)(LSB)
            address addr = MSB + LSB;

            byte RegA = getop(RAM, log);
            byte RegB = getop(RAM, log);

            switch (RegA) {
                case 0x00: {
                    // A source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "JG @%04X, %%A, %%A\n", addr);
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "JG @%04X, %%A, %%B\n", addr);
                            if (A > B) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "JG @%04X, %%A, %%C\n", addr);
                            if (A > C) {
                                *(IP_p) = addr;
                            }
                        }
                        case 0x03: {
                            fprintf(log, "JG @%04X, %%A, %%D\n", addr);
                            if (A > D) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }   
                    }
                    break;
                }
                case 0x01: {
                    // B source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "JG @%04X, %%B, %%A\n", addr);
                            if (B > A) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "JG @%04X, %%B, %%B\n", addr);
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "JG @%04X, %%B, %%C\n", addr);
                            if (B > C) {
                                *(IP_p) = addr;
                            }
                        }
                        case 0x03: {
                            fprintf(log, "JG @%04X, %%B, %%D\n", addr);
                            if (B > D) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x02: {
                    // C source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "JG @%04X, %%C, %%A\n", addr);
                            if (C > A) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "JG @%04X, %%C, %%B\n", addr);
                            if (C > B) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "JG @%04X, %%C, %%C\n", addr);
                        }
                        case 0x03: {
                            fprintf(log, "JG @%04X, %%C, %%D\n", addr);
                            if (C > D) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x03: {
                    // D source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "JG @%04X, %%D, %%A\n", addr);
                            if (D > A) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "JG @%04X, %%D, %%B\n", addr);
                            if (D > B) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "JG @%04X, %%D, %%C\n", addr);
                            if (D > C) {
                                *(IP_p) = addr;
                            }
                        }
                        case 0x03: {
                            fprintf(log, "JG @%04X, %%D, %%D\n", addr);
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }   
                    }
                }
                default: {
                    fprintf(log, "Invalid A register, 0x%02X!\n", RegA);
                    break;
                }
            }
        }

        case 0x2C: {
            // Halves of the address

            byte LSB = getop(RAM, log);
            address MSB = getop(RAM, log);
            // shift the MSB 8 bits left so (--------)00000000 is MSB
            MSB = MSB << 8;
            // add the MSB (--------)00000000 and LSB 00000000(--------) to make (MSB)(LSB)
            address addr = MSB + LSB;

            byte RegA = getop(RAM, log);
            byte RegB = getop(RAM, log);

            switch (RegA) {
                case 0x00: {
                    // A source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "JL @%04X, %%A, %%A\n", addr);
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "JL @%04X, %%A, %%B\n", addr);
                            if (A < B) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "JL @%04X, %%A, %%C\n", addr);
                            if (A < C) {
                                *(IP_p) = addr;
                            }
                        }
                        case 0x03: {
                            fprintf(log, "JL @%04X, %%A, %%D\n", addr);
                            if (A < D) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }   
                    }
                    break;
                }
                case 0x01: {
                    // B source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "JL @%04X, %%B, %%A\n", addr);
                            if (B < A) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "JL @%04X, %%B, %%B\n", addr);
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "JL @%04X, %%B, %%C\n", addr);
                            if (B < C) {
                                *(IP_p) = addr;
                            }
                        }
                        case 0x03: {
                            fprintf(log, "JL @%04X, %%B, %%D\n", addr);
                            if (B < D) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x02: {
                    // C source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "JL @%04X, %%C, %%A\n", addr);
                            if (C < A) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "JL @%04X, %%C, %%B\n", addr);
                            if (C < B) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "JL @%04X, %%C, %%C\n", addr);
                        }
                        case 0x03: {
                            fprintf(log, "JL @%04X, %%C, %%D\n", addr);
                            if (C < D) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x03: {
                    // D source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(log, "JL @%04X, %%D, %%A\n", addr);
                            if (D < A) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(log, "JL @%04X, %%D, %%B\n", addr);
                            if (D < B) {
                                *(IP_p) = addr;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(log, "JL @%04X, %%D, %%C\n", addr);
                            if (D < C) {
                                *(IP_p) = addr;
                            }
                        }
                        case 0x03: {
                            fprintf(log, "JL @%04X, %%D, %%D\n", addr);
                            break;
                        }
                        default: {
                            fprintf(log, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }   
                    }
                }
                default: {
                    fprintf(log, "Invalid A register, 0x%02X!\n", RegA);
                    break;
                }
            }
        }
    }
}
void print_op(memory RAM, byte op, FILE* log) {
    switch (op) {
        case 0x2D: {
            byte val = getop(RAM, log);
            write(RAM, 0xFFFD, val);
            *(S_p) = S | 0x20;
            fprintf(log, "PRNI $%02X\n", val);
            break;
        }

        case 0x2E: {
            byte REG = getop(RAM, log);
            switch (REG) {
                case 0x00: {
                    write(RAM, 0xFFFD, A);
                    *(S_p) = S | 0x20;
                    fprintf(log, "PRNR %%A\n");
                    break;
                }

                case 0x01: {
                    write(RAM, 0xFFFD, B);
                    *(S_p) = S | 0x20;
                    fprintf(log, "PRNR %%B\n");
                    break;
                }

                case 0x02: {
                    write(RAM,0xFFFD, C);
                    *(S_p) = S | 0x20;
                    fprintf(log, "PRNR %%C\n");
                    break;
                }

                case 0x03: {
                    write(RAM, 0xFFFD, D);
                    *(S_p) = S | 0x20;
                    fprintf(log, "PRNR %%D\n");
                    break;
                }
            
                default:
                    fprintf(log, "Invalid register, 0x%02X!\n", REG);
                    break;
            }
            break;
        }

        case 0x2F: {
            fprintf(log, "ENPR\n");
            *(S_p) = S & 0xDF;
            break;
        }
    }
}

void init(memory RAM) {
    // Get the start address
    address LSB = read(RAM, 0xFFFE);
    address MSB = read(RAM, 0xFFFF);
    // shift the MSB 8 bits left so (--------)00000000 is MSB
    MSB = MSB << 8;
    // add the MSB (--------)00000000 and LSB 00000000(--------) to make (MSB)(LSB)
    FILE* output = fopen("OUTPUT.txt", "w");
    FILE* log = fopen("LOG.txt", "w");
    (*IP_p) = MSB + LSB;
    while ((S & 0x80) == 0x00) {
        if ((S & 0x20) == 0x20) {
            fprintf(output, "%c", read(RAM, 0xFFFD));
        }
        byte opcode = getop(RAM, log);
        int type = op_type(opcode, log);
        switch (type) {
            case -1:
                continue;
                break;

            case 0:
                mov_op(RAM, opcode, log);
                break;

            case 1:
                mem_op(RAM, opcode, log);
                break;

            case 2:
                math_op(RAM, opcode, log);
                break;

            case 3:
                stack_op(RAM, opcode, log);
                break;

            case 4:
                jump_op(RAM, opcode, log);
                break;

            case 5:
                halt_op(log);
                break;

            case 6:
                over_op(RAM, opcode, log);
                break;

            case 7:
                status_op(RAM, opcode, log);
                break;

            case 8:
                logic_op(RAM, opcode, log);
                break;
            case 9:
                equal_op(RAM, opcode, log);
                break;
            case 10:
                ineq_op(RAM, opcode, log);
                break;
            case 11:
                print_op(RAM, opcode, log);
        }
    }
    fclose(log);
    fclose(output);
    return;
}
