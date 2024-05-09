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
                            if (((address) A+A) > 0xFF) {
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
                            if (((address) A+B) > 0xFF) {
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
                            if (((address) A+C) > 0xFF) {
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
                            if (((address) A+D) > 0xFF) {
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
                            if (((address) B+A) > 0xFF) {
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
                            if (((address) B+B) > 0xFF) {
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
                            if (((address) B+C) > 0xFF) {
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
                            if (((address) B+D) > 0xFF) {
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
                            if (((address) C+A) > 0xFF) {
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
                            if (((address) C+B) > 0xFF) {
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
                            if (((address) C+C) > 0xFF) {
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
                            if (((address) C+D) > 0xFF) {
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
                            if (((address) D+A) > 0xFF) {
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
                            if (((address) D+B) > 0xFF) {
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
                            if (((address) D+C) > 0xFF) {
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
                            if (((address) D+D) > 0xFF) {
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
                            if (((address) A-A) > 0xFF) {
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
                            if (((address) A-B) > 0xFF) {
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
                            if (((address) A-C) > 0xFF) {
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
                            if (((address) A-D) > 0xFF) {
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
                            if (((address) B-A) > 0xFF) {
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
                            if (((address) B-B) > 0xFF) {
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
                            if (((address) B-C) > 0xFF) {
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
                            if (((address) B-D) > 0xFF) {
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
                            if (((address) C-A) > 0xFF) {
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
                            if (((address) C-B) > 0xFF) {
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
                            if (((address) C-C) > 0xFF) {
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
                            if (((address) C-D) > 0xFF) {
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
                            if (((address) D-A) > 0xFF) {
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
                            if (((address) D-B) > 0xFF) {
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
                            if (((address) D-C) > 0xFF) {
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
                            if (((address) D-D) > 0xFF) {
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
