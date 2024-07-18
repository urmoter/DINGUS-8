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
byte flip_byte(byte data) {
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

byte getop(memory RAM) {
    byte opcode = read(RAM, IP);
    fprintf(stdout, "byte 0x%02X at address 0x%04X\n", opcode, IP);
    *(IP_p) = IP + 1;
    return opcode;
}

int op_type(byte opcode) {
    if (opcode == 0x00) {
        fprintf(stdout, "NOP\n");
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

    fprintf(stderr, "Not a valid opcode!! (0x%02X)\n", opcode);
    return -1;
}

void mov_op(memory RAM, byte op) {
    switch (op) {
        case 0x01: {
            byte data = getop(RAM);
            fprintf(stdout, "MOVA $%02X\n", data);
            (*A_p) = data;
            if (A == 0x00) {
                (*S_p) = S | 0x02;
            }
            break;
        }
        case 0x02: {
            byte data = getop(RAM);
            fprintf(stdout, "MOVB $%02X\n", data);
            (*B_p) = data;
            if (B == 0x00) {
                (*S_p) = S | 0x02;
            }
            break;
        }
        case 0x03: {
            byte data = getop(RAM);
            fprintf(stdout, "MOVA 0x%02X\n", data);
            (*C_p) = data;
            if (C == 0x00) {
                (*S_p) = S | 0x02;
            }
            break;
        }
        case 0x04: {
            byte data = getop(RAM);
            fprintf(stdout, "MOVA 0x%02X\n", data);
            (*D_p) = data;
            if (D == 0x00) {
                (*S_p) = S | 0x02;
            }
            break;
        }
        case 0x05: {
            byte RegA = getop(RAM);
            byte RegB = getop(RAM);
            // Here we go!
            // Switchception!!
            switch (RegA) {
                case 0x00: {
                    // A source cases
                    switch (RegB) {
                        fprintf(stdout, "REG A: %02X\nREG B: %02X\n", RegA, RegB);
                        case 0x00: {
                            fprintf(stdout, "CPY %%A, %%A\n");
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(stdout, "CPY %%A, %%B\n");
                            (*B_p) = A;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(stdout, "CPY %%A, %%C\n");
                            (*C_p) = A;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(stdout, "CPY %%A, %%D\n");
                            (*D_p) = A;
                            if (D == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        default: {
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }   
                    }
                    break;
                }
                case 0x01: {
                    // B source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "CPY %%B, %%A\n");
                            (*A_p) = B;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(stdout, "CPY %%B, %%B\n");
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(stdout, "CPY %%B, %%C\n");
                            (*C_p) = B;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(stdout, "CPY %%B, %%D\n");
                            (*D_p) = B;
                            if (D == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        default: {
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x02: {
                    // C source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "CPY %%C, %%A\n");
                            (*A_p) = C;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(stdout, "CPY %%C, %%B\n");
                            (*B_p) = C;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(stdout, "CPY %%C, %%C\n");
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(stdout, "CPY %%C, %%D\n");
                            (*D_p) = C;
                            if (D == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        default: {
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x03: {
                    // D source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "CPY %%D, %%A\n");
                            (*A_p) = D;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(stdout, "CPY %%D, %%B\n");
                            (*B_p) = D;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(stdout, "CPY %%D, %%C\n");
                            (*C_p) = D;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(stdout, "CPY %%D, %%D\n");
                            if (D == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        default: {
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                default: {
                    fprintf(stdout, "Invalid A register, 0x%02X!\n", RegA);
                    break;
                }
            }
            break;
        }
    }
}
void mem_op(memory RAM, byte op) {
    switch (op) {
        case 0x06: {
            // Source Register
            byte Reg = getop(RAM);
            // Halves of the address
            byte LSB = getop(RAM);
            byte MSB = getop(RAM);
             // shift the MSB 8 bits left so (--------)00000000 is MSB
            MSB = MSB << 8;
            // add the MSB (--------)00000000 and LSB 00000000(--------) to make (MSB)(LSB)
            address addr = MSB + LSB;

            switch (Reg) {
                case 0x00: {
                    fprintf(stdout, "STR %%A, @%04X\n", addr);
                    write(RAM, addr, A);
                    if (A == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }
                case 0x01: {
                    fprintf(stdout, "STR %%B, @%04X\n", addr);
                    write(RAM, addr, B);
                    if (B == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }
                case 0x02: {
                    fprintf(stdout, "STR %%C, @%04X\n", addr);
                    write(RAM, addr, C);
                    if (C == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }
                case 0x03: {
                    fprintf(stdout, "STR %%D, @%04X\n", addr);
                    write(RAM, addr, D);
                    if (D == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }

                default: {
                    fprintf(stdout, "Invalid register, 0x%02X!\n", Reg);
                    break;
                }
            }
            break;
        }

        case 0x07: {
            // Halves of the address
            byte LSB = getop(RAM);
            byte MSB = getop(RAM);
             // shift the MSB 8 bits left so (--------)00000000 is MSB
            MSB = MSB << 8;
            // add the MSB (--------)00000000 and LSB 00000000(--------) to make (MSB)(LSB)
            address addr = MSB + LSB;
            // Destination Register
            byte Reg = getop(RAM);

            switch (Reg) {
                case 0x00: {
                    fprintf(stdout, "LDR @%04X, %%A\n", addr);
                    (*A_p) = read(RAM, addr);
                    if (A == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }
                case 0x01: {
                    fprintf(stdout, "LDR @%04X, %%B\n", addr);
                    (*B_p) = read(RAM, addr);
                    if (B == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }
                case 0x02: {
                    fprintf(stdout, "LDR @%04X, %%C\n", addr);
                    (*C_p) = read(RAM, addr);
                    if (C == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }
                case 0x03: {
                    fprintf(stdout, "LDR @%04X, %%D\n", addr);
                    (*D_p) = read(RAM, addr);
                    if (D == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }

                default: {
                    fprintf(stdout, "Invalid register, 0x%02X!\n", Reg);
                    break;
                }
            }
        }
    }
}
void math_op(memory RAM, byte op) {
    switch (op) {
        case 0x08: {
            byte RegA = getop(RAM);
            byte RegB = getop(RAM);
            // Here we go!
            // Switchception!!
            switch (RegA) {
                case 0x00: {
                    // A source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "ADD %%A, %%A\n");
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
                            fprintf(stdout, "ADD %%A, %%B\n");
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
                            fprintf(stdout, "ADD %%A, %%C\n");
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
                            fprintf(stdout, "ADD %%A, %%D\n");
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
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x01: {
                    // B source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "ADD %%B, %%A\n");
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
                            fprintf(stdout, "ADD %%B, %%B\n");
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
                            fprintf(stdout, "ADD %%B, %%C\n");
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
                            fprintf(stdout, "ADD %%B, %%D\n");
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
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x02: {
                    // C source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "ADD %%C, %%A\n");
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
                            fprintf(stdout, "ADD %%C, %%B\n");
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
                            fprintf(stdout, "ADD %%C, %%C\n");
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
                            fprintf(stdout, "ADD %%C, %%D\n");
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
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x03: {
                    // D source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "ADD %%D, %%A\n");
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
                            fprintf(stdout, "ADD %%D, %%B\n");
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
                            fprintf(stdout, "ADD %%D, %%C\n");
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
                            fprintf(stdout, "ADD %%D, %%D\n");
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
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                }
                default: {
                    fprintf(stdout, "Invalid A register, 0x%02X!\n", RegA);
                    break;
                }
            }
            break;
        }
        case 0x09: {
            byte RegA = getop(RAM);
            byte RegB = getop(RAM);
            // Here we go!
            // Switchception!!
            switch (RegA) {
                case 0x00: {
                    // A source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "SUB %%A, %%A\n");
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
                            fprintf(stdout, "SUB %%A, %%B\n");
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
                            fprintf(stdout, "SUB %%A, %%C\n");
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
                            fprintf(stdout, "SUB %%A, %%D\n");
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
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x01: {
                    // B source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "SUB %%B, %%A\n");
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
                            fprintf(stdout, "SUB %%B, %%B\n");
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
                            fprintf(stdout, "SUB %%B, %%C\n");
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
                            fprintf(stdout, "SUB %%B, %%D\n");
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
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x02: {
                    // C source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "SUB %%C, %%A\n");
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
                            fprintf(stdout, "SUB %%C, %%B\n");
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
                            fprintf(stdout, "SUB %%C, %%C\n");
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
                            fprintf(stdout, "SUB %%C, %%D\n");
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
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x03: {
                    // D source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "SUB %%D, %%A\n");
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
                            fprintf(stdout, "SUB %%D, %%B\n");
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
                            fprintf(stdout, "SUB %%D, %%C\n");
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
                            fprintf(stdout, "SUB %%D, %%D\n");
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
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                default: {
                    fprintf(stdout, "Invalid A register, 0x%02X!\n", RegA);
                    break;
                }
            }
            break;
        }
        case 0x0A: {
            byte reg = getop(RAM);
            switch (reg) {
                case 0x00: {
                    fprintf(stdout, "INC %%A\n");
                    if (A == 0xFF) {
                        (*S_p) = S | 0x12;
                    }
                    (*A_p) = A + 1;
                    break;
                }
                case 0x01: {
                    fprintf(stdout, "INC %%B\n");
                    if (B == 0xFF) {
                        (*S_p) = S | 0x12;
                    }
                    (*B_p) = B + 1;
                    break;
                }
                case 0x02: {
                    fprintf(stdout, "INC %%C\n");
                    if (C == 0xFF) {
                        (*S_p) = S | 0x12;
                    }
                    (*C_p) = C + 1;
                    break;
                }
                case 0x03: {
                    fprintf(stdout, "INC %%D\n");
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
            byte reg = getop(RAM);
            switch (reg) {
                fprintf(stdout, "DEC %%A\n");
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
                    fprintf(stdout, "DEC %%B\n");
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
                    fprintf(stdout, "DEC %%C\n");
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
                    fprintf(stdout, "DEC %%D\n");
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
            byte RegA = getop(RAM);
            byte RegB = getop(RAM);
            // Here we go!
            // Switchception!!
            switch (RegA) {
                case 0x00: {
                    // A source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "ADC %%A, %%A\n");
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
                            fprintf(stdout, "ADC %%A, %%B\n");
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
                            fprintf(stdout, "ADC %%A, %%C\n");
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
                            fprintf(stdout, "ADC %%A, %%D\n");
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
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x01: {
                    // B source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "ADC %%B, %%A\n");
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
                            fprintf(stdout, "ADC %%B, %%B\n");
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
                            fprintf(stdout, "ADC %%B, %%C\n");
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
                            fprintf(stdout, "ADC %%B, %%D\n");
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
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x02: {
                    // C source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "ADC %%C, %%A\n");
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
                            fprintf(stdout, "ADC %%C, %%B\n");
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
                            fprintf(stdout, "ADC %%C, %%C\n");
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
                            fprintf(stdout, "ADC %%C, %%D\n");
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
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x03: {
                    // D source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "ADC %%D, %%A\n");
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
                            fprintf(stdout, "ADC %%D, %%B\n");
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
                            fprintf(stdout, "ADC %%D, %%C\n");
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
                            fprintf(stdout, "ADC %%D, %%D\n");
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
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                default: {
                    fprintf(stdout, "Invalid A register, 0x%02X!\n", RegA);
                    break;
                }
            }
            break;
        }
        case 0x0D: {
            byte borrow = ((S & 0x04) == 0x04) ? 0x01 : 0x00;
            byte RegA = getop(RAM);
            byte RegB = getop(RAM);
            // Here we go!
            // Switchception!!
            switch (RegA) {
                case 0x00: {
                    // A source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "SBB %%A, %%A\n");
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
                            fprintf(stdout, "SBB %%A, %%B\n");
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
                            fprintf(stdout, "SBB %%A, %%C\n");
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
                            fprintf(stdout, "SBB %%A, %%D\n");
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
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x01: {
                    // B source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "SBB %%B, %%A\n");
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
                            fprintf(stdout, "SBB %%B, %%B\n");
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
                            fprintf(stdout, "SBB %%B, %%C\n");
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
                            fprintf(stdout, "SBB %%B, %%D\n");
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
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x02: {
                    // C source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "SBB %%C, %%A\n");
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
                            fprintf(stdout, "SBB %%C, %%B\n");
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
                            fprintf(stdout, "SBB %%C, %%C\n");
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
                            fprintf(stdout, "SBB %%C, %%D\n");
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
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                case 0x03: {
                    // D source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "SBB %%D, %%A\n");
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
                            fprintf(stdout, "SBB %%D, %%B\n");
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
                            fprintf(stdout, "SBB %%D, %%C\n");
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
                            fprintf(stdout, "SBB %%D, %%D\n");
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
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                    break;
                }
                default: {
                    fprintf(stdout, "Invalid A register, 0x%02X!\n", RegA);
                    break;
                }
            }
            break;
        }
        case 0x0E: {
            byte reg = getop(RAM);
            switch (reg) {
                case 0x00: {
                    fprintf(stdout, "NEG %%A\n");
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
                    fprintf(stdout, "NEG %%B\n");
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
                    fprintf(stdout, "NEG %%C\n");
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
                    fprintf(stdout, "NEG %%D\n");
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
                    fprintf(stdout, "0x%02X is an invalid register!\n", reg);
                    break;
                }
            }
            break;
        }
        case 0x0F: {
            byte reg = getop(RAM);
            switch (reg) {
                case 0x00: {
                    fprintf(stdout, "FLP %%A\n");
                    (*A_p) = flip_byte(A);
                    if (A >= 0x80) {
                        (*S_p) = S | 0x04;
                    }
                    if (A == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }

                case 0x01: {
                    fprintf(stdout, "FLP %%B\n");
                    (*B_p) = flip_byte(B);
                    if (B >= 0x80) {
                        (*S_p) = S | 0x04;
                    }
                    if (B == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }

                case 0x02: {
                    fprintf(stdout, "FLP %%C\n");
                    (*C_p) = flip_byte(C);
                    if (C >= 0x80) {
                        (*S_p) = S | 0x04;
                    }
                    if (C == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }

                case 0x03: {
                    fprintf(stdout, "FLP %%D\n");
                    (*D_p) = flip_byte(D);
                    if (D >= 0x80) {
                        (*S_p) = S | 0x04;
                    }
                    if (D == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }

                default: {
                    fprintf(stdout, "0x%02X is an invalid register!\n", reg);
                    break;
                }
            }
            break;
        }
    }
}
void stack_op(memory RAM, byte op) {
    switch (op) {
        case 0x10: {
            byte data = getop(RAM);
            write(RAM, (address) SP, data);
            *(SP_p) = SP - 1;
            fprintf(stdout, "PSHI $%02X\n", data);
            break;
        }

        case 0x11: {
            byte reg = getop(RAM);
            switch (reg) {
                case 0x00: {
                    write(RAM, (address) SP, A);
                    *(SP_p) = SP - 1;
                    fprintf(stdout, "PSHR %%A\n");
                    break;
                }

                case 0x01: {
                    write(RAM, (address) SP, B);
                    *(SP_p) = SP - 1;
                    fprintf(stdout, "PSHR %%B\n");
                    break;
                }

                case 0x02: {
                    write(RAM, (address) SP, C);
                    *(SP_p) = SP - 1;
                    fprintf(stdout, "PSHR %%C\n");
                    break;
                }

                case 0x03: {
                    write(RAM, (address) SP, D);
                    *(SP_p) = SP - 1;
                    fprintf(stdout, "PSHR %%D\n");
                    break;
                }

                default: {
                    fprintf(stdout, "Invalid register, 0x%02X!\n", reg);
                    break;
                }
            }

            break;
        }

        case 0x12: {
            byte reg = getop(RAM);
            switch (reg) {
                case 0x00: {
                    *(SP_p) = SP + 1;
                    *(A_p) = read(RAM, SP);
                    write(RAM, SP, 0x00);
                    fprintf(stdout, "POP %%A\n");
                    break;
                }

                case 0x01: {
                    *(SP_p) = SP + 1;
                    *(B_p) = read(RAM, SP);
                    write(RAM, SP, 0x00);
                    fprintf(stdout, "POP %%B\n");
                    break;
                }

                case 0x02: {
                    *(SP_p) = SP + 1;
                    *(C_p) = read(RAM, SP);
                    write(RAM, SP, 0x00);
                    fprintf(stdout, "POP %%C\n");
                    break;
                }

                case 0x03: {
                    *(SP_p) = SP + 1;
                    *(D_p) = read(RAM, SP);
                    write(RAM, SP, 0x00);
                    fprintf(stdout, "POP %%D\n");
                    break;
                }

                default: {
                    fprintf(stdout, "Invalid register, 0x%02X!\n", reg);
                    break;
                }
            }

            break;
        }
    }
}
void jump_op(memory RAM, byte op) {
    switch (op) {
        case 0x13: {
            // Halves of the address
            byte LSB = getop(RAM);
            byte MSB = getop(RAM);
             // shift the MSB 8 bits left so (--------)00000000 is MSB
            MSB = MSB << 8;
            // add the MSB (--------)00000000 and LSB 00000000(--------) to make (MSB)(LSB)
            address addr = MSB + LSB;
            *(IP_p) = addr;
            fprintf(stdout, "JMP $%04X\n", addr);
            break;
        }

        case 0x14: {
            // Halves of the address
            byte LSB = getop(RAM);
            byte MSB = getop(RAM);
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
            fprintf(stdout, "CALL $%04X\n", addr);
            break;
        }

        case 0x15: {
            // Popping  IP
            *(SP_p) = SP + 1;
            byte MSB = read(RAM, SP);
            write(RAM, SP, 0x00);

            *(SP_p) = SP + 1;
            byte LSB = read(RAM, SP);
            write(RAM, SP, 0x00);

            MSB = MSB << 8;
            
            *(IP_p) = MSB;
            fprintf(stdout, "RET\n");
        }
    
        case 0x16: {
            // Halves of the address
            byte LSB = getop(RAM);
            byte MSB = getop(RAM);
            // shift the MSB 8 bits left so (--------)00000000 is MSB
            MSB = MSB << 8;
            // add the MSB (--------)00000000 and LSB 00000000(--------) to make (MSB)(LSB)
            address addr = MSB + LSB;
            if ((S & 0x02) == 0) {
                *(IP_p) = addr;
                fprintf(stdout, "JZ $%04X\n", addr);
                break;
            } else {
                fprintf(stdout, "JZ $%04X\n", addr);
                break;
            }
        }

        case 0x17: {
            // Halves of the address
            byte LSB = getop(RAM);
            byte MSB = getop(RAM);
            // shift the MSB 8 bits left so (--------)00000000 is MSB
            MSB = MSB << 8;
            // add the MSB (--------)00000000 and LSB 00000000(--------) to make (MSB)(LSB)
            address addr = MSB + LSB;
            if ((S & 0x02) != 0) {
                *(IP_p) = addr;
                fprintf(stdout, "JNZ $%04X\n", addr);
                break;
            } else {
                fprintf(stdout, "JNZ $%04X\n", addr);
                break;
            }
        }
    }
}
void halt_op() {
    fprintf(stdout, "General Purpose Registers:\nA: %02X\nB: %02X\nC: %02X\nD: %02X\n", A, B, C, D);
    fprintf(stdout, "Special Registers:\nS: %02X\nSP: %04X\nIP: %04X\n", S, SP, IP);

    *(S_p) = S | 0x80;
    return;
}
void over_op(memory RAM, byte op) {
    switch (op) {
        case 0x19: {
            // Halves of the address
            byte LSB = getop(RAM);
            byte MSB = getop(RAM);
            // shift the MSB 8 bits left so (--------)00000000 is MSB
            MSB = MSB << 8;
            // add the MSB (--------)00000000 and LSB 00000000(--------) to make (MSB)(LSB)
            address addr = MSB + LSB;
            if ((S & 0x01) == 0) {
                *(IP_p) = addr;
                fprintf(stdout, "JO $%04X\n", addr);
                break;
            } else {
                fprintf(stdout, "JO $%04X\n", addr);
                break;
            }
        }

        case 0x1A: {
            // Halves of the address
            byte LSB = getop(RAM);
            byte MSB = getop(RAM);
            // shift the MSB 8 bits left so (--------)00000000 is MSB
            MSB = MSB << 8;
            // add the MSB (--------)00000000 and LSB 00000000(--------) to make (MSB)(LSB)
            address addr = MSB + LSB;
            if ((S & 0x01) != 0) {
                *(IP_p) = addr;
                fprintf(stdout, "JNO $%04X\n", addr);
                break;
            } else {
                fprintf(stdout, "JNO $%04X\n", addr);
                break;
            }
        }
    }
}
void status_op(memory RAM, byte op) {
    switch (op) {
        // Overflow
        case 0x1B: {
            *(S_p) = S | 0x01;
            fprintf(stdout, "SETO\n");
            break;
        }

        case 0x1C: {
            *(S_p) = S & 0xFE;
            fprintf(stdout, "CLRO\n");
            break;
        }

        // Zero
        case 0x1D: {
            *(S_p) = S | 0x02;
            fprintf(stdout, "SETZ\n");
            break;
        }

        case 0x1E: {
            *(S_p) = S & 0xF;
            fprintf(stdout, "CLRZ\n");
            break;
        }

        // Negative
        case 0x1F: {
            *(S_p) = S | 0x04;
            fprintf(stdout, "SETN\n");
            break;
        }

        case 0x20: {
            *(S_p) = S & 0xFB;
            fprintf(stdout, "CLRN\n");
            break;
        }

        // Parity
        case 0x21: {
            *(S_p) = S | 0x08;
            fprintf(stdout, "SETP\n");
            break;
        }

        case 0x22: {
            *(S_p) = S & 0xF7;
            fprintf(stdout, "CLRP\n");
            break;
        }

        // Carry
        case 0x23: {
            *(S_p) = S | 0x10;
            fprintf(stdout, "SETC\n");
            break;
        }

        case 0x24: {
            *(S_p) = S & 0xEF;
            fprintf(stdout, "CLRC\n");
            break;
        }
    }
}
void logic_op(memory RAM, byte op) {
    switch (op) {
        case 0x25: {
            byte RegA = getop(RAM);
            byte RegB = getop(RAM);
            // Here we go!
            // Switchception!!
            switch (RegA) {
                case 0x00: {
                    // A source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "AND %%A, %%A");
                            break;
                        }
                        case 0x01: {
                            fprintf(stdout, "AND %%A, %%B");
                            *(A_p) = A & B;
                            break;
                        }
                        case 0x02: {
                            fprintf(stdout, "AND %%A, %%C");
                            *(A_p) = A & C;
                            break;
                        }
                        case 0x03: {
                            fprintf(stdout, "AND %%A, %%D");
                            *(A_p) = A & D;
                            break;
                        }
                        default: {
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                }
                case 0x01: {
                    // B source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "AND %%B, %%A");
                            *(B_p) = B & A;
                            break;
                        }
                        case 0x01: {
                            fprintf(stdout, "AND %%B, %%B");\
                            break;
                        }
                        case 0x02: {
                            fprintf(stdout, "AND %%B, %%C");
                            *(B_p) = B & C;
                            break;
                        }
                        case 0x03: {
                            fprintf(stdout, "AND %%B, %%D");
                            *(B_p) = B & D;
                            break;
                        }
                        default: {
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                }
                case 0x02: {
                    // C source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "AND %%C, %%A");
                            *(C_p) = C & A;
                            break;
                        }
                        case 0x01: {
                            fprintf(stdout, "AND %%C, %%B");
                            *(C_p) = C & B;
                            break;
                        }
                        case 0x02: {
                            fprintf(stdout, "AND %%C, %%C");
                            break;
                        }
                        case 0x03: {
                            fprintf(stdout, "AND %%C, %%D");
                            *(C_p) = C & D;
                            break;
                        }
                        default: {
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                }
                case 0x03: {
                    // D source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "AND %%D, %%A");
                            *(D_p) = D & A;
                            break;
                        }
                        case 0x01: {
                            fprintf(stdout, "AND %%D, %%B");
                            *(D_p) = D & B;
                            break;
                        }
                        case 0x02: {
                            fprintf(stdout, "AND %%D, %%C");
                            *(D_p) = D & C;
                            break;
                        }
                        case 0x03: {
                            fprintf(stdout, "AND %%D, %%D");
                            break;
                        }
                        default: {
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                }
                default: {
                    fprintf(stdout, "Invalid A register, 0x%02X!\n", RegA);
                    break;
                }
            }
            break;
        }

        case 0x26: {
            byte RegA = getop(RAM);
            byte RegB = getop(RAM);
            // Here we go!
            // Switchception!!
            switch (RegA) {
                case 0x00: {
                    // A source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "OR %%A, %%A");
                            break;
                        }
                        case 0x01: {
                            fprintf(stdout, "OR %%A, %%B");
                            *(A_p) = A | B;
                            break;
                        }
                        case 0x02: {
                            fprintf(stdout, "OR %%A, %%C");
                            *(A_p) = A | C;
                            break;
                        }
                        case 0x03: {
                            fprintf(stdout, "OR %%A, %%D");
                            *(A_p) = A | D;
                            break;
                        }
                        default: {
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                }
                case 0x01: {
                    // B source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "OR %%B, %%A");
                            *(B_p) = B | A;
                            break;
                        }
                        case 0x01: {
                            fprintf(stdout, "OR %%B, %%B");\
                            break;
                        }
                        case 0x02: {
                            fprintf(stdout, "OR %%B, %%C");
                            *(B_p) = B | C;
                            break;
                        }
                        case 0x03: {
                            fprintf(stdout, "OR %%B, %%D");
                            *(B_p) = B | D;
                            break;
                        }
                        default: {
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                }
                case 0x02: {
                    // C source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "OR %%C, %%A");
                            *(C_p) = C | A;
                            break;
                        }
                        case 0x01: {
                            fprintf(stdout, "OR %%C, %%B");
                            *(C_p) = C | B;
                            break;
                        }
                        case 0x02: {
                            fprintf(stdout, "OR %%C, %%C");
                            break;
                        }
                        case 0x03: {
                            fprintf(stdout, "OR %%C, %%D");
                            *(C_p) = C | D;
                            break;
                        }
                        default: {
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                }
                case 0x03: {
                    // D source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "OR %%D, %%A");
                            *(D_p) = D | A;
                            break;
                        }
                        case 0x01: {
                            fprintf(stdout, "OR %%D, %%B");
                            *(D_p) = D | B;
                            break;
                        }
                        case 0x02: {
                            fprintf(stdout, "OR %%D, %%C");
                            *(D_p) = D | C;
                            break;
                        }
                        case 0x03: {
                            fprintf(stdout, "OR %%D, %%D");
                            break;
                        }
                        default: {
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                }
                default: {
                    fprintf(stdout, "Invalid A register, 0x%02X!\n", RegA);
                    break;
                }
            }
            break;
        }

        case 0x27: {
            byte RegA = getop(RAM);
            byte RegB = getop(RAM);
            // Here we go!
            // Switchception!!
            switch (RegA) {
                case 0x00: {
                    // A source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "XOR %%A, %%A");
                            break;
                        }
                        case 0x01: {
                            fprintf(stdout, "XOR %%A, %%B");
                            *(A_p) = A ^ B;
                            break;
                        }
                        case 0x02: {
                            fprintf(stdout, "XOR %%A, %%C");
                            *(A_p) = A ^ C;
                            break;
                        }
                        case 0x03: {
                            fprintf(stdout, "XOR %%A, %%D");
                            *(A_p) = A ^ D;
                            break;
                        }
                        default: {
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                }
                case 0x01: {
                    // B source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "XOR %%B, %%A");
                            *(B_p) = B ^ A;
                            break;
                        }
                        case 0x01: {
                            fprintf(stdout, "XOR %%B, %%B");\
                            break;
                        }
                        case 0x02: {
                            fprintf(stdout, "XOR %%B, %%C");
                            *(B_p) = B ^ C;
                            break;
                        }
                        case 0x03: {
                            fprintf(stdout, "XOR %%B, %%D");
                            *(B_p) = B ^ D;
                            break;
                        }
                        default: {
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                }
                case 0x02: {
                    // C source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "XOR %%C, %%A");
                            *(C_p) = C ^ A;
                            break;
                        }
                        case 0x01: {
                            fprintf(stdout, "XOR %%C, %%B");
                            *(C_p) = C ^ B;
                            break;
                        }
                        case 0x02: {
                            fprintf(stdout, "XOR %%C, %%C");
                            break;
                        }
                        case 0x03: {
                            fprintf(stdout, "XOR %%C, %%D");
                            *(C_p) = C ^ D;
                            break;
                        }
                        default: {
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                }
                case 0x03: {
                    // D source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "XOR %%D, %%A");
                            *(D_p) = D ^ A;
                            break;
                        }
                        case 0x01: {
                            fprintf(stdout, "XOR %%D, %%B");
                            *(D_p) = D ^ B;
                            break;
                        }
                        case 0x02: {
                            fprintf(stdout, "XOR %%D, %%C");
                            *(D_p) = D ^ C;
                            break;
                        }
                        case 0x03: {
                            fprintf(stdout, "XOR %%D, %%D");
                            break;
                        }
                        default: {
                            fprintf(stdout, "Invalid B register, 0x%02X!\n", RegB);
                            break;
                        }
                    }
                }
                default: {
                    fprintf(stdout, "Invalid A register, 0x%02X!\n", RegA);
                    break;
                }
            }
            break;
        }
    
        case 0x28: {
            byte Reg = getop(RAM);

            switch (Reg) {
                case 0x00: {
                    fprintf(stdout, "NOT %%A\n");
                    *(A_p) = ~A;
                    break;
                }

                case 0x01: {
                    fprintf(stdout, "NOT %%B\n");
                    *(B_p) = ~B;
                    break;
                }

                case 0x02: {
                    fprintf(stdout, "NOT %%C\n");
                    *(C_p) = ~C;
                    break;
                }

                case 0x03: {
                    fprintf(stdout, "NOT %%D\n");
                    *(D_p) = ~D;
                    break;
                }

                default: {
                    fprintf(stdout, "Invalid register, 0x%02X!\n", Reg);
                    break;
                }
            }
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
    (*IP_p) = MSB + LSB;
    while ((S & 0x80) == 0x00) {
        byte opcode = getop(RAM);
        int type = op_type(opcode);
        switch (type) {
            case -1:
                continue;
                break;

            case 0:
                mov_op(RAM, opcode);
                break;

            case 1:
                mem_op(RAM, opcode);
                break;

            case 2:
                math_op(RAM, opcode);
                break;

            case 3:
                stack_op(RAM, opcode);
                break;

            case 4:
                jump_op(RAM, opcode);
                break;

            case 5:
                halt_op();
                break;

            case 6:
                over_op(RAM, opcode);
                break;

            case 7:
                status_op(RAM, opcode);
                break;

            case 8:
                logic_op(RAM, opcode);
                break;
        }
    }
    return;
}
