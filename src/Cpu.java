import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;


// Merry Christmas, or whatever it is, most of these comments are a retrospective of my sleep-deprived code from 2 A.M.
// I am scared.


enum OpcodeType {
    NOP,
    MOV,
    MEM,
    MTH,
    STK,
    JMP,
    END,
    LSS,
    STT,
    LOG,
    EQU,
    MSC,
    PRN,
    CAR, // The Ultimate Life Form
    NEG,
    GRT,
    TMP,
    OFS,
    WID,
    CMP,
}

public class Cpu {
    private final Memory RAM = new Memory();
    private int A = 0;
    private int B = 0;
    private int C = 0;
    private int D = 0;
    private int S = 0;
    private int IP;
    private int SP = 0xFF;
    private int BP = SP;
    private int R = 0x0000;

    public Cpu(String bin_file) {
        // Declare the transfer variable
        byte data;
        // Attempt to open the provided Simulated Ram Binary File
        File bin = new File(bin_file);

        // Try to read from the file
        try (FileInputStream stream = new FileInputStream(bin)) {
            // Set an index for the file (address in ram)
            int i = 0;
            // write each byte of the file to the Emulated Ram
            while ((data = (byte) stream.read()) != -1) {
                RAM.write(data, i++);
            }
        // How did this even fucking happen???
        } catch (IOException error) {
            System.out.println("Uh Oh! We had an issue reading the binary file!");
            throw new RuntimeException(error);
        }
        // Let's start this shit!
        IP = get_start_address();
    }

    public void start() {
        // set the holder for data read from memory
        int data;
        /*
          set the type variable, to hold what type
          of operation the data value is
        */
        OpcodeType type;

        // Run this as long as:
        //  1: We haven't wrapped around to 0x0000
        //  2: The exit flag isn't set
        while (((S & 0x80) == 0) && (IP <= 0xFFFF)) {
            // Get a byte from memory
            data = get_byte();
            // Get what type of operation the byte is
            type = determine_opcode_type(data);
            // If the terminal flag is set, and we haven't wrapped around,
            // print the character in the terminal address.

            // DEBUG
            // System.out.println("Address: " + String.format("0x%04X", IP-1));
            // System.out.println("Byte " + String.format("0x%02X", data) + "; Type " + type);

            if (((S &= 0x20) != 0x00) && IP <= 0xFFFF) {
                System.out.print((char) RAM.read(0xFFFD));
            }
            // Execute the operation
            type_exec(data, type);
        }
    }

    private void type_exec(int data, OpcodeType type) {
        switch (type) {
            case NOP -> {}
            case MOV -> exec_MOV(data);
            case MEM -> exec_MEM(data);
            case MTH -> exec_MTH(data);
            case STK -> exec_STK(data);
            case JMP -> exec_JMP(data);
            case END -> exec_END();
            case LSS -> exec_LSS(data);
            case STT -> exec_STT(data);
            case LOG -> exec_LOG(data);
            case EQU -> exec_EQU(data);
            case MSC -> exec_MSC(data);
            case PRN -> exec_PRN(data);
            case CAR -> exec_CAR(data);
            case NEG -> exec_NEG(data);
            case GRT -> exec_GRT(data);
            case TMP -> exec_TMP();
            case OFS -> exec_OFS();
            case WID -> exec_WID(data);
            case CMP -> exec_CMP(data);
        }
    }

    private int get_start_address() {
        // return the combined values for [0xFFFF] + [0xFFFE]
        int LSB = RAM.read(0xFFFE);
        int MSB = RAM.read(0xFFFF);
        // Debug for when shit hits the fan
        // System.out.printf("Start: 0x%04X%n", (MSB << 8) + LSB);
        return (MSB << 8) + LSB;
    }

    private int get_byte() {
        int data =  RAM.read(IP);
        // Debug for when shit hits the fan
        // System.out.printf("0x%04X%n", IP);
        IP++;
        return data;
    }

    private OpcodeType determine_opcode_type(int opcode) throws RuntimeException {
        return switch (opcode) {
            case 0x00 -> OpcodeType.NOP;
            case 0x01, 0x02, 0x03, 0x04, 0x05 -> OpcodeType.MOV;
            case 0x06, 0x07 -> OpcodeType.MEM;
            case 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0F -> OpcodeType.MTH;
            case 0x10, 0x11, 0x12 -> OpcodeType.STK;
            case 0x13, 0x14, 0x15, 0x16, 0x17 -> OpcodeType.JMP;
            case 0x18 -> OpcodeType.END;
            case 0x19, 0x1A -> OpcodeType.LSS;
            case 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24 -> OpcodeType.STT;
            case 0x25, 0x26, 0x27, 0x28 -> OpcodeType.LOG;
            case 0x29, 0x2A -> OpcodeType.EQU;
            case 0x2B, 0x2C -> OpcodeType.MSC;
            case 0x2D, 0x2E, 0x2F -> OpcodeType.PRN;
            case 0x30, 0x31 -> OpcodeType.CAR;
            case 0x32, 0x33 -> OpcodeType.NEG;
            case 0x34, 0x35 -> OpcodeType.GRT;
            case 0x36 -> OpcodeType.TMP;
            case 0x37 -> OpcodeType.OFS;
            case 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41 -> OpcodeType.WID;
            case 0x42, 0x43, 0x44 -> OpcodeType.CMP;
            default -> throw new RuntimeException("INVALID OPCODE " + String.format("0x%02X", opcode) + "!");
        };
    }

    private void exec_MOV(int opcode) {
        switch (opcode) {
            // MOVA
            case 0x01 -> {A = get_byte(); checkZero(A); }
            // MOVB
            case 0x02 -> {B = get_byte(); checkZero(B); }
            // MOVC
            case 0x03 -> {C = get_byte(); checkZero(C); }
            // MOVD
            case 0x04 -> {D = get_byte(); checkZero(D); }
            // CPY
            case 0x05 -> {
                int regA = get_byte();
                int regB = get_byte();

                switch (regA) {
                    case 0x00 -> {
                        switch (regB) {
                            case 0x00 -> {checkZero(A); }
                            case 0x01 -> {B = A; checkZero(B); }
                            case 0x02 -> {C = A; checkZero(C); }
                            case 0x03 -> {D = A; checkZero(D); }
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x00 -> {A = B; checkZero(A); }
                            case 0x01 -> {checkZero(B); }
                            case 0x02 -> {C = B; checkZero(C); }
                            case 0x03 -> {D = B; checkZero(D); }
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        switch (regB) {
                            case 0x00 -> {A = C; checkZero(A); }
                            case 0x01 -> {B = C; checkZero(B); }
                            case 0x02 -> {checkZero(C); }
                            case 0x03 -> {D = C; checkZero(D); }
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x00 -> {A = D; checkZero(A); }
                            case 0x01 -> {B = D; checkZero(B); }
                            case 0x02 -> {C = D; checkZero(C); }
                            case 0x03 -> {checkZero(D); }
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                }
            }
        }
    }

    private void exec_MEM(int opcode) {
        switch (opcode) {
            // STR
            case 0x06 -> {
                int reg = get_byte();
                int addr = (get_byte() + (get_byte() << 8));

                switch (reg) {
                    case 0x00 -> {RAM.write(A, addr); checkZero(RAM.read(addr));}
                    case 0x01 -> {RAM.write(B, addr); checkZero(RAM.read(addr));}
                    case 0x02 -> {RAM.write(C, addr); checkZero(RAM.read(addr));}
                    case 0x03 -> {RAM.write(D, addr); checkZero(RAM.read(addr));}
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
            // LDR
            case 0x07 -> {
                int addr = (get_byte() + (get_byte() << 8));
                int reg = get_byte();

                switch (reg) {
                    case 0x00 -> {A = RAM.read(addr); checkZero(A); }
                    case 0x01 -> {B = RAM.read(addr); checkZero(B); }
                    case 0x02 -> {C = RAM.read(addr); checkZero(C); }
                    case 0x03 -> {D = RAM.read(addr); checkZero(D); }
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
        }
    }

    private void exec_MTH(int opcode) {

        int carry = ((S & 0x10) != 0x00) ? 1 : 0;
        int negative = ((S & 0x04) != 0x00) ? 1 : 0;
        switch (opcode) {
            // ADD
            case 0x08 -> {
                int regA = get_byte();
                int regB = get_byte();

                switch (regA) {
                    case 0x00 -> {
                        switch (regB) {
                            case 0x00 -> {A += (byte) A; checkZero(A);  checkCarry(A);}
                            case 0x01 -> {A += (byte) B; checkZero(A);  checkCarry(A);}
                            case 0x02 -> {A += (byte) C; checkZero(A);  checkCarry(A);}
                            case 0x03 -> {A += (byte) D; checkZero(A);  checkCarry(A);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x00 -> {B += (byte) A; checkZero(B);  checkCarry(B);}
                            case 0x01 -> {B += (byte) B; checkZero(B);  checkCarry(B);}
                            case 0x02 -> {B += (byte) C; checkZero(B);  checkCarry(B);}
                            case 0x03 -> {B += (byte) D; checkZero(B);  checkCarry(B);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        switch (regB) {
                            case 0x00 -> {C += (byte) A; checkZero(C);  checkCarry(C);}
                            case 0x01 -> {C += (byte) B; checkZero(C);  checkCarry(C);}
                            case 0x02 -> {C += (byte) C; checkZero(C);  checkCarry(C);}
                            case 0x03 -> {C += (byte) D; checkZero(C);  checkCarry(C);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x00 -> {D += (byte) A; checkZero(D);  checkCarry(D);}
                            case 0x01 -> {D += (byte) B; checkZero(D);  checkCarry(D);}
                            case 0x02 -> {D += (byte) C; checkZero(D);  checkCarry(D);}
                            case 0x03 -> {D += (byte) D; checkZero(D);  checkCarry(D);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
            // SUB
            case 0x09 -> {
                int regA = get_byte();
                int regB = get_byte();

                switch (regA) {
                    case 0x00 -> {
                        switch (regB) {
                            case 0x00 -> {A -= (byte) A; checkZero(A);  checkNegative(A);}
                            case 0x01 -> {A -= (byte) B; checkZero(A);  checkNegative(A);}
                            case 0x02 -> {A -= (byte) C; checkZero(A);  checkNegative(A);}
                            case 0x03 -> {A -= (byte) D; checkZero(A);  checkNegative(A);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x00 -> {B -= (byte) A; checkZero(B);  checkNegative(B);}
                            case 0x01 -> {B -= (byte) B; checkZero(B);  checkNegative(B);}
                            case 0x02 -> {B -= (byte) C; checkZero(B);  checkNegative(B);}
                            case 0x03 -> {B -= (byte) D; checkZero(B);  checkNegative(B);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        switch (regB) {
                            case 0x00 -> {C -= (byte) A; checkZero(C);  checkNegative(C);}
                            case 0x01 -> {C -= (byte) B; checkZero(C);  checkNegative(C);}
                            case 0x02 -> {C -= (byte) C; checkZero(C);  checkNegative(C);}
                            case 0x03 -> {C -= (byte) D; checkZero(C);  checkNegative(C);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x00 -> {D -= (byte) A; checkZero(D);  checkNegative(D);}
                            case 0x01 -> {D -= (byte) B; checkZero(D);  checkNegative(D);}
                            case 0x02 -> {D -= (byte) C; checkZero(D);  checkNegative(D);}
                            case 0x03 -> {D -= (byte) D; checkZero(D);  checkNegative(D);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
            // INC
            case 0x0A -> {
                int reg = get_byte();
                switch (reg) {
                    case 0x00 -> {A++; A = (byte) A; checkZero(A);  checkCarry(A);}
                    case 0x01 -> {B++; B = (byte) B; checkZero(B);  checkCarry(B);}
                    case 0x02 -> {C++; C = (byte) C; checkZero(C);  checkCarry(C);}
                    case 0x03 -> {D++; D = (byte) D; checkZero(D);  checkCarry(D);}
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
            // DEC
            case 0x0B -> {
                int reg = get_byte();
                switch (reg) {
                    case 0x00 -> {A--; A = (byte) A; checkZero(A);  checkNegative(A);}
                    case 0x01 -> {B--; B = (byte) B; checkZero(B);  checkNegative(B);}
                    case 0x02 -> {C--; C = (byte) C; checkZero(C);  checkNegative(C);}
                    case 0x03 -> {D--; D = (byte) D; checkZero(D);  checkNegative(D);}
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
            // ADC
            case 0x0C -> {
                int regA = get_byte();
                int regB = get_byte();

                switch (regA) {
                    case 0x00 -> {
                        switch (regB) {
                            case 0x00 -> {A += (byte) A + carry; checkZero(A);  checkCarry(A);}
                            case 0x01 -> {A += (byte) B + carry; checkZero(A);  checkCarry(A);}
                            case 0x02 -> {A += (byte) C + carry; checkZero(A);  checkCarry(A);}
                            case 0x03 -> {A += (byte) D + carry; checkZero(A);  checkCarry(A);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x00 -> {B += (byte) A + carry; checkZero(B);  checkCarry(B);}
                            case 0x01 -> {B += (byte) B + carry; checkZero(B);  checkCarry(B);}
                            case 0x02 -> {B += (byte) C + carry; checkZero(B);  checkCarry(B);}
                            case 0x03 -> {B += (byte) D + carry; checkZero(B);  checkCarry(B);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        switch (regB) {
                            case 0x00 -> {C += (byte) A + carry; checkZero(C);  checkCarry(C);}
                            case 0x01 -> {C += (byte) B + carry; checkZero(C);  checkCarry(C);}
                            case 0x02 -> {C += (byte) C + carry; checkZero(C);  checkCarry(C);}
                            case 0x03 -> {C += (byte) D + carry; checkZero(C);  checkCarry(C);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x00 -> {D += (byte) A + carry; checkZero(D);  checkCarry(D);}
                            case 0x01 -> {D += (byte) B + carry; checkZero(D);  checkCarry(D);}
                            case 0x02 -> {D += (byte) C + carry; checkZero(D);  checkCarry(D);}
                            case 0x03 -> {D += (byte) D + carry; checkZero(D);  checkCarry(D);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
            // SBB
            case 0x0D -> {
                int regA = get_byte();
                int regB = get_byte();

                switch (regA) {
                    case 0x00 -> {
                        switch (regB) {
                            case 0x00 -> {A -= (byte) (A + negative); checkZero(A);  checkNegative(A);}
                            case 0x01 -> {A -= (byte) (B + negative); checkZero(A);  checkNegative(A);}
                            case 0x02 -> {A -= (byte) (C + negative); checkZero(A);  checkNegative(A);}
                            case 0x03 -> {A -= (byte) (D + negative); checkZero(A);  checkNegative(A);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x00 -> {B -= (byte) (A + negative); checkZero(B);  checkNegative(B);}
                            case 0x01 -> {B -= (byte) (B + negative); checkZero(B);  checkNegative(B);}
                            case 0x02 -> {B -= (byte) (C + negative); checkZero(B);  checkNegative(B);}
                            case 0x03 -> {B -= (byte) (D + negative); checkZero(B);  checkNegative(B);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        switch (regB) {
                            case 0x00 -> {C -= (byte) (A + negative); checkZero(C);  checkNegative(C);}
                            case 0x01 -> {C -= (byte) (B + negative); checkZero(C);  checkNegative(C);}
                            case 0x02 -> {C -= (byte) (C + negative); checkZero(C);  checkNegative(C);}
                            case 0x03 -> {C -= (byte) (D + negative); checkZero(C);  checkNegative(C);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x00 -> {D -= (byte) (A + negative); checkZero(D);  checkNegative(D);}
                            case 0x01 -> {D -= (byte) (B + negative); checkZero(D);  checkNegative(D);}
                            case 0x02 -> {D -= (byte) (C + negative); checkZero(D);  checkNegative(D);}
                            case 0x03 -> {D -= (byte) (D + negative); checkZero(D);  checkNegative(D);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
            // NEG
            case 0x0E -> {
                int reg = get_byte();
                switch (reg) {
                    case 0x00 -> {A = (byte) ~A + 1; checkZero(A);  checkNegative(A);}
                    case 0x01 -> {B = (byte) ~B + 1; checkZero(B);  checkNegative(B);}
                    case 0x02 -> {C = (byte) ~C + 1; checkZero(C);  checkNegative(C);}
                    case 0x03 -> {D = (byte) ~D + 1; checkZero(D);  checkNegative(D);}
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
            // FLP
            case 0x0F -> {
                int reg = get_byte();
                switch (reg) {
                    case 0x00 -> {A = flip_byte((byte) A); checkZero(A);  checkNegative(A);}
                    case 0x01 -> {B = flip_byte((byte) B); checkZero(B);  checkNegative(B);}
                    case 0x02 -> {C = flip_byte((byte) C); checkZero(C);  checkNegative(C);}
                    case 0x03 -> {D = flip_byte((byte) D); checkZero(D);  checkNegative(D);}
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
        }
    }

    private void exec_STK(int opcode) {
        switch (opcode) {
            // PSHI
            case 0x10 -> {
                int data = get_byte();
                push_stack(data);
            }
            // PSHR
            case 0x11 -> {
                int reg = get_byte();
                switch (reg) {
                    case 0x00 -> push_stack(A);
                    case 0x01 -> push_stack(B);
                    case 0x02 -> push_stack(C);
                    case 0x03 -> push_stack(D);
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
            // POP
            case 0x12 -> {
                int reg = get_byte();
                switch (reg) {
                    case 0x00 -> {A = pop_stack(); checkZero(A); }
                    case 0x01 -> {B = pop_stack(); checkZero(B); }
                    case 0x02 -> {C = pop_stack(); checkZero(C); }
                    case 0x03 -> {D = pop_stack(); checkZero(D); }
                    default -> throw new RuntimeException("INVALID REGISTER");
                }}
        }
    }

    private void exec_JMP(int opcode) {
        switch (opcode) {
            // JMP
            case 0x13 -> IP = (get_byte() + (get_byte() << 8));
            // CALL
            case 0x14 -> {
                int dest = (get_byte() + (get_byte() << 8));
                // Set return register to the next instruction
                R = IP++;
                // Set base pointer for Arg reference
                BP = SP;
                // Get to the called code
                IP = dest;
            }
            // RET
            case 0x15 -> {
                // Set SP back to BP (incase any local variables were made)
                SP = BP;
                // Return to the caller
                IP = R;
            }
            // JZ
            case 0x16 -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & 0x02) != 0x00) {
                    IP = addr;
                }
            }
            // JNZ
            case 0x17 -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & 0x02) == 0x00) {
                    IP = addr;
                }
            }
        }
    }

    private void exec_END() {
        // END
        S |= 0x80;
        System.out.println("%A: " + String.format("0x%02X", A));
        System.out.println("%B: " + String.format("0x%02X", B));
        System.out.println("%C: " + String.format("0x%02X", C));
        System.out.println("%D: " + String.format("0x%02X", D));
        System.out.println("%S: " + String.format("0x%02X", S));
        System.out.println("@IP: " + String.format("0x%04X", (IP - 1)));
        System.out.println("%SP: " + String.format("0x%02X", SP));
        System.out.println("%BP: " + String.format("0x%02X", BP));
    }

    private void exec_LSS(int opcode) {
        switch (opcode) {
            // JL
            case 0x19 -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & 0x01) != 0x00) {
                    IP = addr;
                }
            }
            // JNL
            case 0x1A -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & 0x01) == 0x00) {
                    IP = addr;
                }
            }
        }
    }

    private void exec_STT(int opcode) {
        switch (opcode) {
            // SETL
            case 0x1B -> S |= 0x01;
            // CLRL
            case 0x1C -> S &= 0xFE;
            // SETZ
            case 0x1D -> S |= 0x02;
            // CLRZ
            case 0x1E -> S &= 0xFD;
            // SETN
            case 0x1F -> S |= 0x04;
            // CLRN
            case 0x20 -> S &= 0xFB;
            // SETG
            case 0x21 -> S |= 0x08;
            // CLRG
            case 0x22 -> S &= 0xF7;
            // SETC
            case 0x23 -> S |= 0x10;
            // CLRC
            case 0x24 -> S &= 0xEF;
        }
    }

    private void exec_LOG(int opcode) {
        switch (opcode) {
            // AND
            case 0x25 -> {
                int regA = get_byte();
                int regB = get_byte();

                switch (regA) {
                    case 0x00 -> {
                        switch (regB) {
                            case 0x00 -> {checkZero(A); }
                            case 0x01 -> {
                                A &= B;
                                checkZero(A);
                                
                            }
                            case 0x02 -> {
                                A &= C;
                                checkZero(A);
                                
                            }
                            case 0x03 -> {
                                A &= D;
                                checkZero(A);
                                
                            }
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x00 -> {
                                B &= A;
                                checkZero(B); 
                            }
                            case 0x01 -> {checkZero(B); }
                            case 0x02 -> {
                                B &= C;
                                checkZero(B); 
                            }
                            case 0x03 -> {
                                B &= D;
                                checkZero(B); 
                            }
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        switch (regB) {
                            case 0x00 -> {
                                C &= A;
                                checkZero(C); 
                            }
                            case 0x01 -> {
                                C &= B;
                                checkZero(C); 
                            }
                            case 0x02 -> {checkZero(C); }
                            case 0x03 -> {
                                C &= D;
                                checkZero(C); 
                            }
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x00 -> {
                                D &= A;
                                checkZero(D); 
                            }
                            case 0x01 -> {
                                D &= B;
                                checkZero(D); 
                            }
                            case 0x02 -> {
                                D &= C;
                                checkZero(D); 
                            }
                            case 0x03 -> {checkZero(D); }
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                }
            }
            // OR
            case 0x26 -> {
                int regA = get_byte();
                int regB = get_byte();

                switch (regA) {
                    case 0x00 -> {
                        switch (regB) {
                            case 0x00 -> {checkZero(A); }
                            case 0x01 -> {
                                A |= B;
                                checkZero(A);
                                
                            }
                            case 0x02 -> {
                                A |= C;
                                checkZero(A);
                                
                            }
                            case 0x03 -> {
                                A |= D;
                                checkZero(A);
                                
                            }
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x00 -> {
                                B |= A;
                                checkZero(B);
                                
                            }
                            case 0x01 -> {checkZero(B); }
                            case 0x02 -> {
                                B |= C;
                                checkZero(B);
                                
                            }
                            case 0x03 -> {
                                B |= D;
                                checkZero(B);
                                
                            }
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        switch (regB) {
                            case 0x00 -> {
                                C |= A;
                                checkZero(C);
                                
                            }
                            case 0x01 -> {
                                C |= B;
                                checkZero(C);
                                
                            }
                            case 0x02 -> {checkZero(C); }
                            case 0x03 -> {
                                C |= D;
                                checkZero(C);
                                
                            }
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x00 -> {
                                D |= A;
                                checkZero(D);
                                
                            }
                            case 0x01 -> {
                                D |= B;
                                checkZero(D);
                                
                            }
                            case 0x02 -> {
                                D |= C;
                                checkZero(D);
                                
                            }
                            case 0x03 -> {checkZero(D); }
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                }
            }
            // XOR
            case 0x27 -> {
                int regA = get_byte();
                int regB = get_byte();

                switch (regA) {
                    case 0x00 -> {
                        switch (regB) {
                            case 0x00 -> {checkZero(A); }
                            case 0x01 -> {
                                A ^= B;
                                checkZero(A);
                                
                            }
                            case 0x02 -> {
                                A ^= C;
                                checkZero(A);
                                
                            }
                            case 0x03 -> {
                                A ^= D;
                                checkZero(A);
                                
                            }
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x00 -> {
                                B ^= A;
                                checkZero(B);
                                
                            }
                            case 0x01 -> {checkZero(B); }
                            case 0x02 -> {
                                B ^= C;
                                checkZero(B);
                                
                            }
                            case 0x03 -> {
                                B ^= D;
                                checkZero(B);
                                
                            }
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        switch (regB) {
                            case 0x00 -> {
                                C ^= A;
                                checkZero(C);
                                
                            }
                            case 0x01 -> {
                                C ^= B;
                                checkZero(C);
                                
                            }
                            case 0x02 -> {checkZero(C); }
                            case 0x03 -> {
                                C ^= D;
                                checkZero(C);
                                
                            }
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x00 -> {
                                D ^= A;
                                checkZero(D);
                                
                            }
                            case 0x01 -> {
                                D ^= B;
                                checkZero(D);
                                
                            }
                            case 0x02 -> {
                                D ^= C;
                                checkZero(D);
                                
                            }
                            case 0x03 -> {checkZero(D); }
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                }
            }
            // NOT
            case 0x28 -> {
                int reg = get_byte();
                switch (reg) {
                    case 0x00 -> {A = ~A; checkZero(A); }
                    case 0x01 -> {B = ~B; checkZero(B); }
                    case 0x02 -> {C = ~C; checkZero(C); }
                    case 0x03 -> {D = ~D; checkZero(D); }
                }
            }
        }
    }

    private void exec_EQU(int opcode) {
        switch (opcode) {
            // JE
            case 0x29 -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & 0x08) != 0x00) {
                    IP = addr;
                }
            }
            // JNE
            case 0x2A -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & 0x08) == 0x00) {
                    IP = addr;
                }
            }
        }
    }

    private void exec_MSC(int opcode) {
        switch (opcode) {
            // CLR
            case 0x2B -> {
                int reg = get_byte();
                switch (reg) {
                    case 0x00 -> {
                        A = 0;
                        checkZero(A);
                    }

                    case 0x01 -> {
                        B = 0;
                        checkZero(B);
                    }

                    case 0x02 -> {
                        C = 0;
                        checkZero(C);
                    }

                    case 0x03 -> {
                        D = 0;
                        checkZero(D);
                    }
                }
            }
            // LEA
            case 0x2C -> {
                int reg16 = get_byte();
                int offset = (get_byte() + (get_byte() << 8));
                int dest = get_byte();

                switch (reg16) {
                    case 0x04 -> {
                        switch (dest) {
                            case 0x00 -> {
                                A = RAM.read(((A << 8) + B)+offset);
                                checkZero(A);
                            }
                            case 0x01 -> {
                                B = RAM.read(((A << 8) + B)+offset);
                                checkZero(B);
                            }
                            case 0x02 -> {
                                C = RAM.read(((A << 8) + B)+offset);
                                checkZero(C);
                            }
                            case 0x03 -> {
                                D = RAM.read(((A << 8) + B)+offset);
                                checkZero(D);
                            }

                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }

                    case 0x05 -> {
                        switch (dest) {
                            case 0x00 -> {
                                A = RAM.read(((C << 8) + D)+offset);
                                checkZero(A);
                            }
                            case 0x01 -> {
                                B = RAM.read(((C << 8) + D)+offset);
                                checkZero(B);
                            }
                            case 0x02 -> {
                                C = RAM.read(((C << 8) + D)+offset);
                                checkZero(C);
                            }
                            case 0x03 -> {
                                D = RAM.read(((C << 8) + D)+offset);
                                checkZero(D);
                            }

                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }

                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
        }
    }

    private void exec_PRN(int opcode) {
        switch (opcode) {
            // PRNI
            case 0x2D -> {
                int data = get_byte();
                RAM.write(data, 0xFFFD);
                startPrint();
            }
            // PRNR
            case 0x2E -> {
                int reg = get_byte();
                switch (reg) {
                    case 0x00 -> {
                        RAM.write(A, 0xFFFD);
                        startPrint();
                    }
                    case 0x01 -> {
                        RAM.write(B, 0xFFFD);
                        startPrint();
                    }
                    case 0x02 -> {
                        RAM.write(C, 0xFFFD);
                        startPrint();
                    }
                    case 0x03 -> {
                        RAM.write(D, 0xFFFD);
                        startPrint();
                    }
                }
            }
            // ENPR
            case 0x2F -> stopPrint();
        }
    }

    private void exec_CAR(int opcode) {
        switch (opcode) {
            // JC
            case 0x30 -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & 0x10) != 0x00) {
                    IP = addr;
                }
            }
            // JNC
            case 0x31 -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & 0x10) == 0x00) {
                    IP = addr;
                }
            }
        }
    }

    private void exec_NEG(int opcode) {
        switch (opcode) {
            // JN
            case 0x32 -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & 0x04) != 0x00) {
                    IP = addr;
                }
            }
            // JNN
            case 0x33 -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & 0x04) == 0x00) {
                    IP = addr;
                }
            }
        }
    }

    private void exec_GRT(int opcode) {
        switch (opcode) {
            // JG
            case 0x34 -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & 0x40) != 0x00) {
                    IP = addr;
                }
            }
            // JNG
            case 0x35 -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & 0x40) == 0x00) {
                    IP = addr;
                }
            }
        }
    }

    private void exec_TMP() {
        // IBPR
        int OFFSET = get_byte();
        int DEST = get_byte();
        switch (DEST) {
            case 0x00 -> {A = RAM.read(BP+OFFSET); checkZero(A); }
            case 0x01 -> {B = RAM.read(BP+OFFSET); checkZero(A); }
            case 0x02 -> {C = RAM.read(BP+OFFSET); checkZero(A); }
            case 0x03 -> {D = RAM.read(BP+OFFSET); checkZero(A); }
        }
    }

    private void exec_OFS() {
        int addr = (get_byte() + (get_byte() << 8));
        int OFFSET = get_byte();
        int dest = get_byte();

        switch (dest) {
            case 0x00 -> {
                switch (OFFSET) {
                    case 0x00 -> {
                        A = RAM.read(addr+A);
                        checkZero(A);
                        
                        checkCarry(A);
                    }
                    case 0x01 -> {
                        A = RAM.read(addr+B);
                        checkZero(A);
                        
                        checkCarry(A);
                    }
                    case 0x02 -> {
                        A = RAM.read(addr+C);
                        checkZero(A);
                        
                        checkCarry(A);
                    }
                    case 0x03 -> {
                        A = RAM.read(addr+D);
                        checkZero(A);
                        
                        checkCarry(A);
                    }
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
            case 0x01 -> {
                switch (OFFSET) {
                    case 0x00 -> {
                        B = RAM.read(addr+A);
                        checkZero(B);
                        
                        checkCarry(B);
                    }
                    case 0x01 -> {
                        B = RAM.read(addr+B);
                        checkZero(B);
                        
                        checkCarry(B);
                    }
                    case 0x02 -> {
                        B = RAM.read(addr+C);
                        checkZero(B);
                        
                        checkCarry(B);
                    }
                    case 0x03 -> {
                        B = RAM.read(addr+D);
                        checkZero(B);
                        
                        checkCarry(B);
                    }
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
            case 0x02 -> {
                switch (OFFSET) {
                    case 0x00 -> {
                        C = RAM.read(addr+A);
                        checkZero(C);
                        
                        checkCarry(C);
                    }
                    case 0x01 -> {
                        C = RAM.read(addr+B);
                        checkZero(C);
                        
                        checkCarry(C);
                    }
                    case 0x02 -> {
                        C = RAM.read(addr+C);
                        checkZero(C);
                        
                        checkCarry(C);
                    }
                    case 0x03 -> {
                        C = RAM.read(addr+D);
                        checkZero(C);
                        
                        checkCarry(C);
                    }
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
            case 0x03 -> {
                switch (OFFSET) {
                    case 0x00 -> {
                        D = RAM.read(addr+A);
                        checkZero(D);
                        
                        checkCarry(D);
                    }
                    case 0x01 -> {
                        D = RAM.read(addr+B);
                        checkZero(D);
                        
                        checkCarry(D);
                    }
                    case 0x02 -> {
                        D = RAM.read(addr+C);
                        checkZero(D);
                        
                        checkCarry(D);
                    }
                    case 0x03 -> {
                        D = RAM.read(addr+D);
                        checkZero(D);
                        
                        checkCarry(D);
                    }
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
            default -> throw new RuntimeException("INVALID REGISTER");
        }
    }

    private void exec_WID(int opcode) {
        switch (opcode) {
            // MOVW
            case 0x38 -> {
                int reg = get_byte();
                int lo = get_byte();
                int hi = get_byte();

                switch (reg) {
                    case 0x04 -> {
                        A = hi; B = lo;
                        checkZero(A); 
                        checkZero(B); 
                    }
                    case 0x05 -> {
                        C = hi; D = lo;
                        checkZero(C); 
                        checkZero(D); 
                    }
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }

            // PSHW
            case 0x39 -> {
                int reg = get_byte();

                switch (reg) {
                    case 0x04 -> {
                        push_stack(A);
                        push_stack(B);
                    }
                    case 0x05 -> {
                        push_stack(C);
                        push_stack(D);
                    }

                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }

            // POPW
            case 0x3A -> {
                int reg = get_byte();

                switch (reg) {
                    case 0x04 -> {
                        B = pop_stack();
                        A = pop_stack();
                        checkZero(A); 
                        checkZero(B); 
                    }
                    case 0x05 -> {
                        C = pop_stack();
                        D = pop_stack();
                        checkZero(C); 
                        checkZero(D); 
                    }

                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }

            // LDW
            case 0x3B -> {
                int regA = get_byte();
                int regB = get_byte();

                switch (regA) {
                    case 0x04 -> {
                        switch (regB) {
                            case 0x00 -> {
                                A = RAM.read((A<<8) + B);
                                checkZero(A); 
                            }
                            case 0x01 -> {
                                B = RAM.read((A<<8) + B);
                                checkZero(B); 
                            }
                            case 0x02 -> {
                                C = RAM.read((A<<8) + B);
                                checkZero(C); 
                            }
                            case 0x03 -> {
                                D = RAM.read((A<<8) + B);
                                checkZero(D); 
                            }

                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x05 -> {
                        switch (regB) {
                            case 0x00 -> {
                                A = RAM.read((C<<8) + D);
                                checkZero(A); 
                            }
                            case 0x01 -> {
                                B = RAM.read((C<<8) + D);
                                checkZero(B); 
                            }
                            case 0x02 -> {
                                C = RAM.read((C<<8) + D);
                                checkZero(C); 
                            }
                            case 0x03 -> {
                                D = RAM.read((C<<8) + D);
                                checkZero(D); 
                            }

                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }

                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }

            // STW
            case 0x3C -> {
                int regA = get_byte();
                int regB = get_byte();

                switch (regA) {
                    case 0x00 -> {
                        switch (regB) {
                            case 0x04 -> {
                                RAM.write(A, (A<<8) + B);
                            }
                            case 0x05 -> {
                                RAM.write(A, (C<<8) + D);
                            }

                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x04 -> {
                                RAM.write(B, (A<<8) + B);
                            }
                            case 0x05 -> {
                                RAM.write(B, (C<<8) + D);
                            }

                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        switch (regB) {
                            case 0x04 -> {
                                RAM.write(C, (A<<8) + B);
                            }
                            case 0x05 -> {
                                RAM.write(C, (C<<8) + D);
                            }

                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x04 -> {
                                RAM.write(D, (A<<8) + B);
                            }
                            case 0x05 -> {
                                RAM.write(D, (C<<8) + D);
                            }

                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }

            // JMPW
            case 0x3D -> {
                int reg = get_byte();

                switch (reg) {
                    case 0x04 -> {
                        IP = (A<<8) + B;
                    }
                    case 0x05 -> {
                        IP = (C<<8) + D;
                    }

                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }

            // CALW
            case 0x3E -> {
                int reg = get_byte();
                switch (reg) {
                    case 0x04 -> {
                        int dest = (A<<8)+B;
                        // Set return register to the next instruction
                        R = IP++;
                        // Set base pointer for Arg reference
                        BP = SP;
                        // Get to the called code
                        IP = dest;
                    }
                    case 0x05 -> {
                        int dest = (C<<8)+D;
                        // Set return register to the next instruction
                        R = IP++;
                        // Set base pointer for Arg reference
                        BP = SP;
                        // Get to the called code
                        IP = dest;
                    }

                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }

            // ADDW
            case 0x3F -> {
                int reg = get_byte();
                int imm = (get_byte() + (get_byte() << 8));

                switch (reg) {
                    case 0x04 -> {
                        int temp_16 = (A<<8)+B;
                        temp_16 += imm;
                        A = (byte) temp_16>>8;
                        B = (byte) temp_16;
                        checkZero(A);  checkCarry(A);
                        checkZero(B);  checkCarry(B);

                    }
                    case 0x05 -> {
                        int temp_16 = (C<<8)+D;
                        temp_16 += imm;
                        C = (byte) temp_16>>8;
                        D = (byte) temp_16;
                        checkZero(C);  checkCarry(C);
                        checkZero(D);  checkCarry(D);
                    }

                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }

            // INCW
            case 0x40 -> {
                int reg = get_byte();

                switch (reg) {
                    case 0x04 -> {
                        int temp_16 = (A<<8)+B;
                        temp_16++;
                        A = (byte) temp_16>>8;
                        B = (byte) temp_16;
                        checkZero(A);  checkCarry(A);
                        checkZero(B);  checkCarry(B);

                    }
                    case 0x05 -> {
                        int temp_16 = (C<<8)+D;
                        temp_16++;
                        C = (byte) temp_16>>8;
                        D = (byte) temp_16;
                        checkZero(C);  checkCarry(C);
                        checkZero(D);  checkCarry(D);
                    }

                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }

            // DECW
            case 0x41 -> {
                int reg = get_byte();

                switch (reg) {
                    case 0x04 -> {
                        int temp_16 = (A<<8)+B;
                        temp_16--;
                        A = (byte) temp_16>>8;
                        B = (byte) temp_16;
                        checkZero(A);  checkNegative(A);
                        checkZero(B);  checkNegative(B);

                    }
                    case 0x05 -> {
                        int temp_16 = (C<<8)+D;
                        temp_16--;
                        C = (byte) temp_16>>8;
                        D = (byte) temp_16;
                        checkZero(C);  checkNegative(C);
                        checkZero(D);  checkNegative(D);
                    }

                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
        }
    }

    private void exec_CMP(int opcode) {
        switch (opcode) {
            // CMP
            case 0x42 -> {
                int regA = get_byte();
                int regB = get_byte();

                switch (regA) {
                    case 0x00 -> {
                        switch (regB) {
                            case 0x00 -> S |= 0x08;

                            case 0x01 -> {
                                if (A == B) {
                                    S |= 0x08; // E
                                    S &= 0xFE; // L
                                    S &= 0xBF; // G
                                } else if (A < B) {
                                    S &= 0xF7; // E
                                    S |= 0x01; // L
                                    S &= 0xBF; // G
                                } else {
                                    S &= 0xF7; // E
                                    S &= 0xFE; // L
                                    S |= 0x40; // G
                                }
                            }
                            case 0x02 -> {
                                if (A == C) {
                                    S |= 0x08; // E
                                    S &= 0xFE; // L
                                    S &= 0xBF; // G
                                } else if (A < C) {
                                    S &= 0xF7; // E
                                    S |= 0x01; // L
                                    S &= 0xBF; // G
                                } else {
                                    S &= 0xF7; // E
                                    S &= 0xFE; // L
                                    S |= 0x40; // G
                                }
                            }
                            case 0x03 -> {
                                if (A == D) {
                                    S |= 0x08; // E
                                    S &= 0xFE; // L
                                    S &= 0xBF; // G
                                } else if (A < D) {
                                    S &= 0xF7; // E
                                    S |= 0x01; // L
                                    S &= 0xBF; // G
                                } else {
                                    S &= 0xF7; // E
                                    S &= 0xFE; // L
                                    S |= 0x40; // G
                                }
                            }

                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x00 -> {
                                if (B == A) {
                                    S |= 0x08; // E
                                    S &= 0xFE; // L
                                    S &= 0xBF; // G
                                } else if (B < A) {
                                    S &= 0xF7; // E
                                    S |= 0x01; // L
                                    S &= 0xBF; // G
                                } else {
                                    S &= 0xF7; // E
                                    S &= 0xFE; // L
                                    S |= 0x40; // G
                                }
                            }
                            case 0x01 -> S |= 0x08;

                            case 0x02 -> {
                                if (B == C) {
                                    S |= 0x08; // E
                                    S &= 0xFE; // L
                                    S &= 0xBF; // G
                                } else if (B < C) {
                                    S &= 0xF7; // E
                                    S |= 0x01; // L
                                    S &= 0xBF; // G
                                } else {
                                    S &= 0xF7; // E
                                    S &= 0xFE; // L
                                    S |= 0x40; // G
                                }
                            }
                            case 0x03 -> {
                                if (B == D) {
                                    S |= 0x08; // E
                                    S &= 0xFE; // L
                                    S &= 0xBF; // G
                                } else if (B < D) {
                                    S &= 0xF7; // E
                                    S |= 0x01; // L
                                    S &= 0xBF; // G
                                } else {
                                    S &= 0xF7; // E
                                    S &= 0xFE; // L
                                    S |= 0x40; // G
                                }
                            }

                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        switch (regB) {
                            case 0x00 -> {
                                if (C == A) {
                                    S |= 0x08; // E
                                    S &= 0xFE; // L
                                    S &= 0xBF; // G
                                } else if (C < A) {
                                    S &= 0xF7; // E
                                    S |= 0x01; // L
                                    S &= 0xBF; // G
                                } else {
                                    S &= 0xF7; // E
                                    S &= 0xFE; // L
                                    S |= 0x40; // G
                                }
                            }
                            case 0x01 -> {
                                if (C == B) {
                                    S |= 0x08; // E
                                    S &= 0xFE; // L
                                    S &= 0xBF; // G
                                } else if (C < B) {
                                    S &= 0xF7; // E
                                    S |= 0x01; // L
                                    S &= 0xBF; // G
                                } else {
                                    S &= 0xF7; // E
                                    S &= 0xFE; // L
                                    S |= 0x40; // G
                                }
                            }
                            case 0x02 -> S |= 0x08;

                            case 0x03 -> {
                                if (C == D) {
                                    S |= 0x08; // E
                                    S &= 0xFE; // L
                                    S &= 0xBF; // G
                                } else if (C < D) {
                                    S &= 0xF7; // E
                                    S |= 0x01; // L
                                    S &= 0xBF; // G
                                } else {
                                    S &= 0xF7; // E
                                    S &= 0xFE; // L
                                    S |= 0x40; // G
                                }
                            }

                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x00 -> {
                                if (D == A) {
                                    S |= 0x08; // E
                                    S &= 0xFE; // L
                                    S &= 0xBF; // G
                                } else if (D < A) {
                                    S &= 0xF7; // E
                                    S |= 0x01; // L
                                    S &= 0xBF; // G
                                } else {
                                    S &= 0xF7; // E
                                    S &= 0xFE; // L
                                    S |= 0x40; // G
                                }
                            }
                            case 0x01 -> {
                                if (D == B) {
                                    S |= 0x08; // E
                                    S &= 0xFE; // L
                                    S &= 0xBF; // G
                                } else if (D < B) {
                                    S &= 0xF7; // E
                                    S |= 0x01; // L
                                    S &= 0xBF; // G
                                } else {
                                    S &= 0xF7; // E
                                    S &= 0xFE; // L
                                    S |= 0x40; // G
                                }
                            }
                            case 0x02 -> {
                                if (D == C) {
                                    S |= 0x08; // E
                                    S &= 0xFE; // L
                                    S &= 0xBF; // G
                                } else if (D < C) {
                                    S &= 0xF7; // E
                                    S |= 0x01; // L
                                    S &= 0xBF; // G
                                } else {
                                    S &= 0xF7; // E
                                    S &= 0xFE; // L
                                    S |= 0x40; // G
                                }
                            }
                            case 0x03 -> S |= 0x08;

                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                }


            }

            // SETG
            case 0x43 -> S |= 0x40;

            // CLRG
            case 0x44 -> S &= 0x40;
        }
    }

    private void checkZero(int data) {
        if (data == 0x00) {
            S |= 0x02;
        } else {
            S &= 0xFD;
        }
    }

    private void checkNegative(int data) {
        if (data > 0xFF) {
            S |= 0x04;
        } else {
            S &= 0xFB;
        }
    }

    private void checkCarry(int data) {
        if (data > 0xFF) {
            S |= 0x10;
        } else {
            S &= 0xEF;
        }
    }
    private int flip_byte(byte data) {
        int intSize = 8;
        byte y = 0;
        for(int position=intSize-1; position>0; position--){
            y += (byte) ((data & 1)<<position);
            data >>= 1;
        }
        return y;
    }

    private void push_stack(int data) {
        RAM.write(data, SP);
        SP--;
    }

    private void startPrint() {
        S|= 0x20;
    }

    private void stopPrint() {
        S &= 0xDF;
    }

    private int pop_stack() {
        SP++;
        int data = RAM.read(SP);
        RAM.write(0x00, SP);
        return data;
    }

}
