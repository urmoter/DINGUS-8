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
            fprintf(stdout, "MOVA 0x%02X\n", data);
            (*A_p) = data;
            if (A == 0x00) {
                (*S_p) = S | 0x02;
            }
            break;
        }
        case 0x02: {
            byte data = getop(RAM);
            fprintf(stdout, "MOVB 0x%02X\n", data);
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
                            fprintf(stdout, "CPY A, A");
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(stdout, "CPY A, B");
                            (*B_p) = A;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(stdout, "CPY A, C");
                            (*C_p) = A;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(stdout, "CPY A, D");
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
                            fprintf(stdout, "CPY B, A");
                            (*A_p) = B;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(stdout, "CPY B, B");
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(stdout, "CPY B, C");
                            (*C_p) = B;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(stdout, "CPY B, D");
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
                            fprintf(stdout, "CPY C, A");
                            (*A_p) = C;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(stdout, "CPY C, B");
                            (*B_p) = C;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(stdout, "CPY C, C");
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(stdout, "CPY C, D");
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
                            fprintf(stdout, "CPY D, A");
                            (*A_p) = D;
                            if (A == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x01: {
                            fprintf(stdout, "CPY D, B");
                            (*B_p) = D;
                            if (B == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x02: {
                            fprintf(stdout, "CPY D, C");
                            (*C_p) = D;
                            if (C == 0x00) {
                                (*S_p) = S | 0x02;
                            }
                            break;
                        }
                        case 0x03: {
                            fprintf(stdout, "CPY D, D");
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
        if ((opcode > 0x00) & (opcode <= 0x05)) {
            mov_op(RAM, opcode);
            continue;
        }
    }
    return;
}
