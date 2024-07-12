#include "../headers/memory.h"
#include <stdio.h>
#include <stdlib.h>

byte A = 0x00;
byte B = 0x00;
byte C = 0x00;
byte D = 0x00;
byte SP = 0x00;
byte S = 0x00;
address IP = 0x0000;

byte *A_p = &A;
byte *B_p = &B;
byte *C_p = &C;
byte *D_p = &D;
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
}

byte getop(memory RAM) {
    byte opcode = read(RAM, IP);
    fprintf(stdout, "byte 0x%02X at address 0x%04X\n", opcode, IP);
    (*IP_p)++;
    return opcode;
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
                        case 0x00: {
                            fprintf(stdout, "CPY %%A, %%A");
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(stdout, "CPY %%A, %%B");
                            (*B_p) = A;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(stdout, "CPY %%A, %%C");
                            (*C_p) = A;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(stdout, "CPY %%A, %%D");
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
                }
                case 0x01: {
                    // B source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "CPY %%B, %%A");
                            (*A_p) = B;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(stdout, "CPY %%B, %%B");
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(stdout, "CPY %%B, %%C");
                            (*C_p) = B;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(stdout, "CPY %%B, %%D");
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
                }
                case 0x02: {
                    // C source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "CPY %%C, %%A");
                            (*A_p) = C;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(stdout, "CPY %%C, %%B");
                            (*B_p) = C;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(stdout, "CPY %%C, %%C");
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(stdout, "CPY %%C, %%D");
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
                }
                case 0x03: {
                    // D source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "CPY %%D, %%A");
                            (*A_p) = D;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(stdout, "CPY %%D, %%B");
                            (*B_p) = D;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(stdout, "CPY %%D, %%C");
                            (*C_p) = D;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(stdout, "CPY %%D, %%D");
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
                    fprintf("STR %%A, @%04X\n", addr);
                    write(RAM, addr, A);
                    if (A == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }
                case 0x01: {
                    fprintf("STR %%B, @%04X\n", addr);
                    write(RAM, addr, B);
                    if (B == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }
                case 0x02: {
                    fprintf("STR %%C, @%04X\n", addr);
                    write(RAM, addr, C);
                    if (C == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }
                case 0x03: {
                    fprintf("STR %%D, @%04X\n", addr);
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
                    fprintf("LDR @%04X, %%A\n", addr);
                    (*A_p) = read(RAM, addr);
                    if (A == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }
                case 0x01: {
                    fprintf("LDR @%04X, %%B\n", addr);
                    (*B_p) = read(RAM, addr);
                    if (B == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }
                case 0x02: {
                    fprintf("LDR @%04X, %%C\n", addr);
                    (*C_p) = read(RAM, addr);
                    if (C == 0x00) {
                        (*S_p) = S | 0x02;
                    }
                    break;
                }
                case 0x03: {
                    fprintf("LDR @%04X, %%D\n", addr);
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
                            fprintf(stdout, "ADD %%A, %%A");
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
                            fprintf(stdout, "ADD %%A, %%B");
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
                            fprintf(stdout, "ADD %%A, %%C");
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
                            fprintf(stdout, "ADD %%A, %%D");
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
                }
                case 0x01: {
                    // B source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "ADD %%B, %%A");
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
                            fprintf(stdout, "ADD %%B, %%B");
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
                            fprintf(stdout, "ADD %%B, %%C");
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
                            fprintf(stdout, "ADD %%B, %%D");
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
                }
                case 0x02: {
                    // C source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "ADD %%C, %%A");
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
                            fprintf(stdout, "ADD %%C, %%B");
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
                            fprintf(stdout, "ADD %%C, %%C");
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
                            fprintf(stdout, "ADD %%C, %%D");
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
                }
                case 0x03: {
                    // D source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "ADD %%D, %%A");
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
                            fprintf(stdout, "ADD %%D, %%B");
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
                            fprintf(stdout, "ADD %%D, %%C");
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
                            fprintf(stdout, "ADD %%D, %%D");
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
                            fprintf(stdout, "SUB %%A, %%A");
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
                            fprintf(stdout, "SUB %%A, %%B");
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
                            fprintf(stdout, "SUB %%A, %%C");
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
                            fprintf(stdout, "SUB %%A, %%D");
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
                }
                case 0x01: {
                    // B source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "SUB %%B, %%A");
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
                            fprintf(stdout, "SUB %%B, %%B");
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
                            fprintf(stdout, "SUB %%B, %%C");
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
                            fprintf(stdout, "SUB %%B, %%D");
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
                }
                case 0x02: {
                    // C source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "SUB %%C, %%A");
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
                            fprintf(stdout, "SUB %%C, %%B");
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
                            fprintf(stdout, "SUB %%C, %%C");
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
                            fprintf(stdout, "SUB %%C, %%D");
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
                }
                case 0x03: {
                    // D source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "SUB %%D, %%A");
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
                            fprintf(stdout, "SUB %%D, %%B");
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
                            fprintf(stdout, "SUB %%D, %%C");
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
                            fprintf(stdout, "SUB %%D, %%D");
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
                    if (A == 0xFF) {
                        (*S_p) = S | 0x12;
                    }
                    (*A_p)++;
                    break;
                }
                case 0x01: {
                    if (B == 0xFF) {
                        (*S_p) = S | 0x12;
                    }
                    (*B_p)++;
                    break;
                }
                case 0x02: {
                    if (C == 0xFF) {
                        (*S_p) = S | 0x12;
                    }
                    (*C_p)++;
                    break;
                }
                case 0x03: {
                    if (D == 0xFF) {
                        (*S_p) = S | 0x12;
                    }
                    (*D_p)++;
                    break;
                }
            }
            break;
        }
        case 0x0B: {
            byte reg = getop(RAM);
            switch (reg) {
                case 0x00: {
                    if (A == 0x00) {
                        (*S_p) = S | 0x10;
                    }
                    (*A_p)--;
                    if (A == 0x00) {
                        (*S_p) = S | 0x04;
                    }
                    break;
                }
                case 0x01: {
                    if (B == 0x00) {
                        (*S_p) = S | 0x10;
                    }
                    (*B_p)--;
                    if (B == 0x00) {
                        (*S_p) = S | 0x04;
                    }
                    break;
                }
                case 0x02: {
                    if (C == 0x00) {
                        (*S_p) = S | 0x10;
                    }
                    (*C_p)--;
                    if (C == 0x00) {
                        (*S_p) = S | 0x04;
                    }
                    break;
                }
                case 0x03: {
                    if (D == 0x00) {
                        (*S_p) = S | 0x10;
                    }
                    (*D_p)--;
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
                            fprintf(stdout, "ADC %%A, %%A");
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
                            fprintf(stdout, "ADC %%A, %%B");
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
                            fprintf(stdout, "ADC %%A, %%C");
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
                            fprintf(stdout, "ADC %%A, %%D");
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
                }
                case 0x01: {
                    // B source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "ADC %%B, %%A");
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
                            fprintf(stdout, "ADC %%B, %%B");
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
                            fprintf(stdout, "ADC %%B, %%C");
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
                            fprintf(stdout, "ADC %%B, %%D");
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
                }
                case 0x02: {
                    // C source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "ADC %%C, %%A");
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
                            fprintf(stdout, "ADC %%C, %%B");
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
                            fprintf(stdout, "ADC %%C, %%C");
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
                            fprintf(stdout, "ADC %%C, %%D");
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
                }
                case 0x03: {
                    // D source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "ADC %%D, %%A");
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
                            fprintf(stdout, "ADC %%D, %%B");
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
                            fprintf(stdout, "ADC %%D, %%C");
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
                            fprintf(stdout, "ADC %%D, %%D");
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
                            fprintf(stdout, "SBB %%A, %%A");
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
                            fprintf(stdout, "SBB %%A, %%B");
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
                            fprintf(stdout, "SBB %%A, %%C");
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
                            fprintf(stdout, "SBB %%A, %%D");
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
                }
                case 0x01: {
                    // B source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "SBB %%B, %%A");
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
                            fprintf(stdout, "SBB %%B, %%B");
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
                            fprintf(stdout, "SBB %%B, %%C");
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
                            fprintf(stdout, "SBB %%B, %%D");
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
                }
                case 0x02: {
                    // C source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "SBB %%C, %%A");
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
                            fprintf(stdout, "SBB %%C, %%B");
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
                            fprintf(stdout, "SBB %%C, %%C");
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
                            fprintf(stdout, "SBB %%C, %%D");
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
                }
                case 0x03: {
                    // D source cases
                    switch (RegB) {
                        case 0x00: {
                            fprintf(stdout, "SBB %%D, %%A");
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
                            fprintf(stdout, "SBB %%D, %%B");
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
                            fprintf(stdout, "SBB %%D, %%C");
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
                            fprintf(stdout, "SBB %%D, %%D");
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
        if (opcode == 0x00) {
            fprintf(stdout, "NOP\n");
            continue;
        }
        if ((opcode > 0x00) && (opcode < 0x06)) {
            mov_op(RAM, opcode);
            continue;
        }
        if ((opcode > 0x05) && (opcode < 0x08)) {
            mem_op(RAM, opcode);
            continue;
        }
        if ((opcode > 0x07) && (opcode < 0x10)) {
            math_op(RAM, opcode);
            continue;
        }
    }
    return;
}
