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

byte flip_byte(byte data) {
    byte* table = {
        0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
        0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
        0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
        0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
        0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
        0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
        0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
        0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
        0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
        0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
        0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
        0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
        0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
        0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
        0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
        0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
        0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
        0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
        0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
        0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
        0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
        0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
        0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
        0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
        0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
        0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
        0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
        0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
        0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
        0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
        0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
        0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
    };
    return table[data];
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
