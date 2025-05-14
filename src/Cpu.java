import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

enum OpcodeType {
    NOP,
    MOV,
    MEM,
    MTH,
    STK,
    JMP,
    END,
    USR,
    STT,
    LOG,
    EQU,
    INQ,
    PRN,
    CAR, // The Ultimate Life Form
    NEG,
    PAR,
    TMP,
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
    private int RR = 0;

    public Cpu(String bin_file) {
        // 
        int arr;
        File bin = new File(bin_file);

        try (FileInputStream stream = new FileInputStream(bin)) {
            int i = 0;

            while ((arr = stream.read()) != -1) {
                RAM.write(arr, i++);
            }

        } catch (IOException error) {
            System.out.println("Uh Oh! We had an issue reading the binary file!");
            throw new RuntimeException(error);
        }

        IP = get_start_address();
    }

    public void start() {
        int data;
        OpcodeType type;

        while (((S & 0x80) == 0) && (IP <= 0xFFFF)) {
            data = get_byte();
            type = determine_opcode_type(data);
            if (((S &= 0x20) != 0x00) && IP <= 0xFFFF) {
                System.out.print(RAM.read(0xFFFD));
            }

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
            case USR -> exec_USR(data);
            case STT -> exec_STT(data);
            case LOG -> exec_LOG(data);
            case EQU -> exec_EQU(data);
            case INQ -> exec_INQ(data);
            case PRN -> exec_PRN(data);
            case CAR -> exec_CAR(data);
            case NEG -> exec_NEG(data);
            case PAR -> exec_PAR(data);
            case TMP -> exec_TMP();
        }
    }

    private int get_start_address() {
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
            case 0x19, 0x1A -> OpcodeType.USR;
            case 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24 -> OpcodeType.STT;
            case 0x25, 0x26, 0x27, 0x28 -> OpcodeType.LOG;
            case 0x29, 0x2A -> OpcodeType.EQU;
            case 0x2B, 0x2C -> OpcodeType.INQ;
            case 0x2D, 0x2E, 0x2F -> OpcodeType.PRN;
            case 0x30, 0x31 -> OpcodeType.CAR;
            case 0x32, 0x33 -> OpcodeType.NEG;
            case 0x34, 0x35 -> OpcodeType.PAR;
            case 0x36 -> OpcodeType.TMP;
            default -> throw new RuntimeException("INVALID OPCODE " + String.format("0x%02X", opcode) + "!");
        };
    }

    private void exec_MOV(int opcode) {
        switch (opcode) {
            // MOVA
            case 0x01 -> {A = get_byte(); checkZero(A); checkParity(A);}
            // MOVB
            case 0x02 -> {B = get_byte(); checkZero(B); checkParity(B);}
            // MOVC
            case 0x03 -> {C = get_byte(); checkZero(C); checkParity(C);}
            // MOVD
            case 0x04 -> {D = get_byte(); checkZero(D); checkParity(D);}
            // CPY
            case 0x05 -> {
                int regA = get_byte();
                int regB = get_byte();

                switch (regA) {
                    case 0x00 -> {
                        switch (regB) {
                            case 0x00 -> {checkZero(A); checkParity(A);}
                            case 0x01 -> {B = A; checkZero(B); checkParity(B);}
                            case 0x02 -> {C = A; checkZero(C); checkParity(C);}
                            case 0x03 -> {D = A; checkZero(D); checkParity(D);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x00 -> {A = B; checkZero(A); checkParity(A);}
                            case 0x01 -> {checkZero(B); checkParity(B);}
                            case 0x02 -> {C = B; checkZero(C); checkParity(C);}
                            case 0x03 -> {D = B; checkZero(D); checkParity(D);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        switch (regB) {
                            case 0x00 -> {A = C; checkZero(A); checkParity(A);}
                            case 0x01 -> {B = C; checkZero(B); checkParity(B);}
                            case 0x02 -> {checkZero(C); checkParity(C);}
                            case 0x03 -> {D = C; checkZero(D); checkParity(D);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x00 -> {A = D; checkZero(A); checkParity(A);}
                            case 0x01 -> {B = D; checkZero(B); checkParity(B);}
                            case 0x02 -> {C = D; checkZero(C); checkParity(C);}
                            case 0x03 -> {checkZero(D); checkParity(D);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                }
            }
        }
    }

    private void exec_MEM(int opcode) {
        switch (opcode) {
            case 0x06 -> {
                int reg = get_byte();
                int addr = (get_byte() + (get_byte() << 8));

                switch (reg) {
                    case 0x00 -> {RAM.write(A, addr); checkZero(RAM.read(addr)); checkParity(RAM.read(addr));}
                    case 0x01 -> {RAM.write(B, addr); checkZero(RAM.read(addr)); checkParity(RAM.read(addr));}
                    case 0x02 -> {RAM.write(C, addr); checkZero(RAM.read(addr)); checkParity(RAM.read(addr));}
                    case 0x03 -> {RAM.write(D, addr); checkZero(RAM.read(addr)); checkParity(RAM.read(addr));}
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }

            case 0x07 -> {
                int addr = (get_byte() + (get_byte() << 8));
                int reg = get_byte();

                switch (reg) {
                    case 0x00 -> {A = RAM.read(addr); checkZero(A); checkParity(A);}
                    case 0x01 -> {B = RAM.read(addr); checkZero(B); checkParity(B);}
                    case 0x02 -> {C = RAM.read(addr); checkZero(C); checkParity(C);}
                    case 0x03 -> {D = RAM.read(addr); checkZero(D); checkParity(D);}
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
        }
    }

    private void exec_MTH(int opcode) {
        int carry = ((S & 0x10) != 0x00) ? 1 : 0;
        int negative = ((S & 0x04) != 0x00) ? 1 : 0;
        switch (opcode) {
            case 0x08 -> {
                int regA = get_byte();
                int regB = get_byte();

                switch (regA) {
                    case 0x00 -> {
                        switch (regB) {
                            case 0x00 -> {A += (byte) A; checkZero(A); checkParity(A); checkCarry(A);}
                            case 0x01 -> {A += (byte) B; checkZero(A); checkParity(A); checkCarry(A);}
                            case 0x02 -> {A += (byte) C; checkZero(A); checkParity(A); checkCarry(A);}
                            case 0x03 -> {A += (byte) D; checkZero(A); checkParity(A); checkCarry(A);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x00 -> {B += (byte) A; checkZero(B); checkParity(B); checkCarry(B);}
                            case 0x01 -> {B += (byte) B; checkZero(B); checkParity(B); checkCarry(B);}
                            case 0x02 -> {B += (byte) C; checkZero(B); checkParity(B); checkCarry(B);}
                            case 0x03 -> {B += (byte) D; checkZero(B); checkParity(B); checkCarry(B);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        switch (regB) {
                            case 0x00 -> {C += (byte) A; checkZero(C); checkParity(C); checkCarry(C);}
                            case 0x01 -> {C += (byte) B; checkZero(C); checkParity(C); checkCarry(C);}
                            case 0x02 -> {C += (byte) C; checkZero(C); checkParity(C); checkCarry(C);}
                            case 0x03 -> {C += (byte) D; checkZero(C); checkParity(C); checkCarry(C);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x00 -> {D += (byte) A; checkZero(D); checkParity(D); checkCarry(D);}
                            case 0x01 -> {D += (byte) B; checkZero(D); checkParity(D); checkCarry(D);}
                            case 0x02 -> {D += (byte) C; checkZero(D); checkParity(D); checkCarry(D);}
                            case 0x03 -> {D += (byte) D; checkZero(D); checkParity(D); checkCarry(D);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
            case 0x09 -> {
                int regA = get_byte();
                int regB = get_byte();

                switch (regA) {
                    case 0x00 -> {
                        switch (regB) {
                            case 0x00 -> {A -= (byte) A; checkZero(A); checkParity(A); checkNegative(A);}
                            case 0x01 -> {A -= (byte) B; checkZero(A); checkParity(A); checkNegative(A);}
                            case 0x02 -> {A -= (byte) C; checkZero(A); checkParity(A); checkNegative(A);}
                            case 0x03 -> {A -= (byte) D; checkZero(A); checkParity(A); checkNegative(A);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x00 -> {B -= (byte) A; checkZero(B); checkParity(B); checkNegative(B);}
                            case 0x01 -> {B -= (byte) B; checkZero(B); checkParity(B); checkNegative(B);}
                            case 0x02 -> {B -= (byte) C; checkZero(B); checkParity(B); checkNegative(B);}
                            case 0x03 -> {B -= (byte) D; checkZero(B); checkParity(B); checkNegative(B);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        switch (regB) {
                            case 0x00 -> {C -= (byte) A; checkZero(C); checkParity(C); checkNegative(C);}
                            case 0x01 -> {C -= (byte) B; checkZero(C); checkParity(C); checkNegative(C);}
                            case 0x02 -> {C -= (byte) C; checkZero(C); checkParity(C); checkNegative(C);}
                            case 0x03 -> {C -= (byte) D; checkZero(C); checkParity(C); checkNegative(C);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x00 -> {D -= (byte) A; checkZero(D); checkParity(D); checkNegative(D);}
                            case 0x01 -> {D -= (byte) B; checkZero(D); checkParity(D); checkNegative(D);}
                            case 0x02 -> {D -= (byte) C; checkZero(D); checkParity(D); checkNegative(D);}
                            case 0x03 -> {D -= (byte) D; checkZero(D); checkParity(D); checkNegative(D);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
            case 0x0A -> {
                int reg = get_byte();
                switch (reg) {
                    case 0x00 -> {A++; A = (byte) A; checkZero(A); checkParity(A); checkCarry(A);}
                    case 0x01 -> {B++; B = (byte) B; checkZero(B); checkParity(B); checkCarry(B);}
                    case 0x02 -> {C++; C = (byte) C; checkZero(C); checkParity(C); checkCarry(C);}
                    case 0x03 -> {D++; D = (byte) D; checkZero(D); checkParity(D); checkCarry(D);}
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
            case 0x0B -> {
                int reg = get_byte();
                switch (reg) {
                    case 0x00 -> {A--; A = (byte) A; checkZero(A); checkParity(A); checkNegative(A);}
                    case 0x01 -> {B--; B = (byte) B; checkZero(B); checkParity(B); checkNegative(B);}
                    case 0x02 -> {C--; C = (byte) C; checkZero(C); checkParity(C); checkNegative(C);}
                    case 0x03 -> {D--; D = (byte) D; checkZero(D); checkParity(D); checkNegative(D);}
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
            case 0x0C -> {
                int regA = get_byte();
                int regB = get_byte();

                switch (regA) {
                    case 0x00 -> {
                        switch (regB) {
                            case 0x00 -> {A += (byte) A + carry; checkZero(A); checkParity(A); checkCarry(A);}
                            case 0x01 -> {A += (byte) B + carry; checkZero(A); checkParity(A); checkCarry(A);}
                            case 0x02 -> {A += (byte) C + carry; checkZero(A); checkParity(A); checkCarry(A);}
                            case 0x03 -> {A += (byte) D + carry; checkZero(A); checkParity(A); checkCarry(A);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x00 -> {B += (byte) A + carry; checkZero(B); checkParity(B); checkCarry(B);}
                            case 0x01 -> {B += (byte) B + carry; checkZero(B); checkParity(B); checkCarry(B);}
                            case 0x02 -> {B += (byte) C + carry; checkZero(B); checkParity(B); checkCarry(B);}
                            case 0x03 -> {B += (byte) D + carry; checkZero(B); checkParity(B); checkCarry(B);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        switch (regB) {
                            case 0x00 -> {C += (byte) A + carry; checkZero(C); checkParity(C); checkCarry(C);}
                            case 0x01 -> {C += (byte) B + carry; checkZero(C); checkParity(C); checkCarry(C);}
                            case 0x02 -> {C += (byte) C + carry; checkZero(C); checkParity(C); checkCarry(C);}
                            case 0x03 -> {C += (byte) D + carry; checkZero(C); checkParity(C); checkCarry(C);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x00 -> {D += (byte) A + carry; checkZero(D); checkParity(D); checkCarry(D);}
                            case 0x01 -> {D += (byte) B + carry; checkZero(D); checkParity(D); checkCarry(D);}
                            case 0x02 -> {D += (byte) C + carry; checkZero(D); checkParity(D); checkCarry(D);}
                            case 0x03 -> {D += (byte) D + carry; checkZero(D); checkParity(D); checkCarry(D);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
            case 0x0D -> {
                int regA = get_byte();
                int regB = get_byte();

                switch (regA) {
                    case 0x00 -> {
                        switch (regB) {
                            case 0x00 -> {A -= (byte) (A + negative); checkZero(A); checkParity(A); checkNegative(A);}
                            case 0x01 -> {A -= (byte) (B + negative); checkZero(A); checkParity(A); checkNegative(A);}
                            case 0x02 -> {A -= (byte) (C + negative); checkZero(A); checkParity(A); checkNegative(A);}
                            case 0x03 -> {A -= (byte) (D + negative); checkZero(A); checkParity(A); checkNegative(A);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x00 -> {B -= (byte) (A + negative); checkZero(B); checkParity(B); checkNegative(B);}
                            case 0x01 -> {B -= (byte) (B + negative); checkZero(B); checkParity(B); checkNegative(B);}
                            case 0x02 -> {B -= (byte) (C + negative); checkZero(B); checkParity(B); checkNegative(B);}
                            case 0x03 -> {B -= (byte) (D + negative); checkZero(B); checkParity(B); checkNegative(B);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        switch (regB) {
                            case 0x00 -> {C -= (byte) (A + negative); checkZero(C); checkParity(C); checkNegative(C);}
                            case 0x01 -> {C -= (byte) (B + negative); checkZero(C); checkParity(C); checkNegative(C);}
                            case 0x02 -> {C -= (byte) (C + negative); checkZero(C); checkParity(C); checkNegative(C);}
                            case 0x03 -> {C -= (byte) (D + negative); checkZero(C); checkParity(C); checkNegative(C);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x00 -> {D -= (byte) (A + negative); checkZero(D); checkParity(D); checkNegative(D);}
                            case 0x01 -> {D -= (byte) (B + negative); checkZero(D); checkParity(D); checkNegative(D);}
                            case 0x02 -> {D -= (byte) (C + negative); checkZero(D); checkParity(D); checkNegative(D);}
                            case 0x03 -> {D -= (byte) (D + negative); checkZero(D); checkParity(D); checkNegative(D);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
            case 0x0E -> {
                int reg = get_byte();
                switch (reg) {
                    case 0x00 -> {A = (byte) ~A + 1; checkZero(A); checkParity(A); checkNegative(A);}
                    case 0x01 -> {B = (byte) ~B + 1; checkZero(B); checkParity(B); checkNegative(B);}
                    case 0x02 -> {C = (byte) ~C + 1; checkZero(C); checkParity(C); checkNegative(C);}
                    case 0x03 -> {D = (byte) ~D + 1; checkZero(D); checkParity(D); checkNegative(D);}
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
            case 0x0F -> {
                int reg = get_byte();
                switch (reg) {
                    case 0x00 -> {A = flip_byte((byte) A); checkZero(A); checkParity(A); checkNegative(A);}
                    case 0x01 -> {B = flip_byte((byte) B); checkZero(B); checkParity(B); checkNegative(B);}
                    case 0x02 -> {C = flip_byte((byte) C); checkZero(C); checkParity(C); checkNegative(C);}
                    case 0x03 -> {D = flip_byte((byte) D); checkZero(D); checkParity(D); checkNegative(D);}
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
        }
    }

    private void exec_STK(int opcode) {
        switch (opcode) {
            case 0x10 -> {
                int data = get_byte();
                push_stack(data);
            }
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
            case 0x12 -> {
                int reg = get_byte();
                switch (reg) {
                    case 0x00 -> {A = pop_stack(); checkZero(A); checkParity(A);}
                    case 0x01 -> {B = pop_stack(); checkZero(B); checkParity(B);}
                    case 0x02 -> {C = pop_stack(); checkZero(C); checkParity(C);}
                    case 0x03 -> {D = pop_stack(); checkZero(D); checkParity(D);}
                    default -> throw new RuntimeException("INVALID REGISTER");
                }}
        }
    }

    private void exec_JMP(int opcode) {
        switch (opcode) {
            case 0x13 -> IP = (get_byte() + (get_byte() << 8));
            case 0x14 -> {
                int dest = (get_byte() + (get_byte() << 8));
                // Set return register to the next instruction
                RR = IP++;
                // Set base pointer for Arg reference
                BP = SP;
                // Get to the called code
                IP = dest;
            }
            case 0x15 -> {
                // Set SP back to BP (incase any local variables were made)
                SP = BP;
                // Return to the caller
                IP = RR;
            }
            case 0x16 -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & 0x02) != 0x00) {
                    IP = addr;
                }
            }
            case 0x17 -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & 0x02) == 0x00) {
                    IP = addr;
                }
            }
        }
    }

    private void exec_END() {
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

    private void exec_USR(int opcode) {
        switch (opcode) {
            case 0x19 -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & 0x01) != 0x00) {
                    IP = addr;
                }
            }
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
            case 0x1B -> S |= 0x01;
            case 0x1C -> S &= 0xFE;
            case 0x1D -> S |= 0x02;
            case 0x1E -> S &= 0xFD;
            case 0x1F -> S |= 0x04;
            case 0x20 -> S &= 0xFB;
            case 0x21 -> S |= 0x08;
            case 0x22 -> S &= 0xF7;
            case 0x23 -> S |= 0x10;
            case 0x24 -> S &= 0xEF;
        }
    }

    private void exec_LOG(int opcode) {
        switch (opcode) {
            case 0x25 -> {
                int regA = get_byte();
                int regB = get_byte();

                switch (regA) {
                    case 0x00 -> {
                        switch (regB) {
                            case 0x00 -> {checkZero(A); checkParity(A);}
                            case 0x01 -> {
                                A &= B;
                                checkZero(A);
                                checkParity(A);
                            }
                            case 0x02 -> {
                                A &= C;
                                checkZero(A);
                                checkParity(A);
                            }
                            case 0x03 -> {
                                A &= D;
                                checkZero(A);
                                checkParity(A);
                            }
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x00 -> {
                                B &= A;
                                checkZero(B); checkParity(B);
                            }
                            case 0x01 -> {checkZero(B); checkParity(B);}
                            case 0x02 -> {
                                B &= C;
                                checkZero(B); checkParity(B);
                            }
                            case 0x03 -> {
                                B &= D;
                                checkZero(B); checkParity(B);
                            }
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        switch (regB) {
                            case 0x00 -> {
                                C &= A;
                                checkZero(C); checkParity(C);
                            }
                            case 0x01 -> {
                                C &= B;
                                checkZero(C); checkParity(C);
                            }
                            case 0x02 -> {checkZero(C); checkParity(C);}
                            case 0x03 -> {
                                C &= D;
                                checkZero(C); checkParity(C);
                            }
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x00 -> {
                                D &= A;
                                checkZero(D); checkParity(D);
                            }
                            case 0x01 -> {
                                D &= B;
                                checkZero(D); checkParity(D);
                            }
                            case 0x02 -> {
                                D &= C;
                                checkZero(D); checkParity(D);
                            }
                            case 0x03 -> {checkZero(D); checkParity(D);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                }
            }
            case 0x26 -> {
                int regA = get_byte();
                int regB = get_byte();

                switch (regA) {
                    case 0x00 -> {
                        switch (regB) {
                            case 0x00 -> {checkZero(A); checkParity(A);}
                            case 0x01 -> {
                                A |= B;
                                checkZero(A);
                                checkParity(A);
                            }
                            case 0x02 -> {
                                A |= C;
                                checkZero(A);
                                checkParity(A);
                            }
                            case 0x03 -> {
                                A |= D;
                                checkZero(A);
                                checkParity(A);
                            }
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x00 -> {
                                B |= A;
                                checkZero(B);
                                checkParity(B);
                            }
                            case 0x01 -> {checkZero(B); checkParity(B);}
                            case 0x02 -> {
                                B |= C;
                                checkZero(B);
                                checkParity(B);
                            }
                            case 0x03 -> {
                                B |= D;
                                checkZero(B);
                                checkParity(B);
                            }
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        switch (regB) {
                            case 0x00 -> {
                                C |= A;
                                checkZero(C);
                                checkParity(C);
                            }
                            case 0x01 -> {
                                C |= B;
                                checkZero(C);
                                checkParity(C);
                            }
                            case 0x02 -> {checkZero(C); checkParity(C);}
                            case 0x03 -> {
                                C |= D;
                                checkZero(C);
                                checkParity(C);
                            }
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x00 -> {
                                D |= A;
                                checkZero(D);
                                checkParity(D);
                            }
                            case 0x01 -> {
                                D |= B;
                                checkZero(D);
                                checkParity(D);
                            }
                            case 0x02 -> {
                                D |= C;
                                checkZero(D);
                                checkParity(D);
                            }
                            case 0x03 -> {checkZero(D); checkParity(D);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                }
            }
            case 0x27 -> {
                int regA = get_byte();
                int regB = get_byte();

                switch (regA) {
                    case 0x00 -> {
                        switch (regB) {
                            case 0x00 -> {checkZero(A); checkParity(A);}
                            case 0x01 -> {
                                A ^= B;
                                checkZero(A);
                                checkParity(A);
                            }
                            case 0x02 -> {
                                A ^= C;
                                checkZero(A);
                                checkParity(A);
                            }
                            case 0x03 -> {
                                A ^= D;
                                checkZero(A);
                                checkParity(A);
                            }
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x00 -> {
                                B ^= A;
                                checkZero(B);
                                checkParity(B);
                            }
                            case 0x01 -> {checkZero(B); checkParity(B);}
                            case 0x02 -> {
                                B ^= C;
                                checkZero(B);
                                checkParity(B);
                            }
                            case 0x03 -> {
                                B ^= D;
                                checkZero(B);
                                checkParity(B);
                            }
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        switch (regB) {
                            case 0x00 -> {
                                C ^= A;
                                checkZero(C);
                                checkParity(C);
                            }
                            case 0x01 -> {
                                C ^= B;
                                checkZero(C);
                                checkParity(C);
                            }
                            case 0x02 -> {checkZero(C); checkParity(C);}
                            case 0x03 -> {
                                C ^= D;
                                checkZero(C);
                                checkParity(C);
                            }
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x00 -> {
                                D ^= A;
                                checkZero(D);
                                checkParity(D);
                            }
                            case 0x01 -> {
                                D ^= B;
                                checkZero(D);
                                checkParity(D);
                            }
                            case 0x02 -> {
                                D ^= C;
                                checkZero(D);
                                checkParity(D);
                            }
                            case 0x03 -> {checkZero(D); checkParity(D);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                }
            }
            case 0x28 -> {
                int reg = get_byte();
                switch (reg) {
                    case 0x00 -> {A = ~A; checkZero(A); checkParity(A);}
                    case 0x01 -> {B = ~B; checkZero(B); checkParity(B);}
                    case 0x02 -> {C = ~C; checkZero(C); checkParity(C);}
                    case 0x03 -> {D = ~D; checkZero(D); checkParity(D);}
                }
            }
        }
    }

    private void exec_EQU(int opcode) {
        switch (opcode) {
            case 0x29 -> {
                int addr = (get_byte() + (get_byte() << 8));
                int regA = get_byte();
                int regB = get_byte();
                switch (regA) {
                    case 0x00 -> {
                        switch (regB) {
                            case 0x00 -> IP = addr;
                            case 0x01 -> {if (A == B) { IP = addr;}}
                            case 0x02 -> {if (A == C) { IP = addr;}}
                            case 0x03 -> {if (A == D) { IP = addr;}}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x00 -> {if (B == A) { IP = addr;}}
                            case 0x01 -> IP = addr;
                            case 0x02 -> {if (B == C) { IP = addr;}}
                            case 0x03 -> {if (B == D) { IP = addr;}}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        // Debug for when shit hits the fan
                        // System.out.printf("0x%02X%n", regB);
                        switch (regB) {
                            case 0x00 -> {if (C == A) { IP = addr;}}
                            case 0x01 -> {if (C == B) { IP = addr;}}
                            case 0x02 -> IP = addr;
                            case 0x03 -> {if (C == D) { IP = addr;}}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x00 -> {if (D == A) { IP = addr;}}
                            case 0x01 -> {if (D == B) { IP = addr;}}
                            case 0x02 -> {if (D == C) { IP = addr;}}
                            case 0x03 -> IP = addr;
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                }
            }
            case 0x2A -> {
                int addr = (get_byte() + (get_byte() << 8));
                int regA = get_byte();
                int regB = get_byte();
                switch (regA) {
                    case 0x00 -> {
                        switch (regB) {
                            case 0x00 -> {}
                            case 0x01 -> {if (A != B) { IP = addr;}}
                            case 0x02 -> {if (A != C) { IP = addr;}}
                            case 0x03 -> {if (A != D) { IP = addr;}}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x00 -> {if (B != A) { IP = addr;}}
                            case 0x01 -> {}
                            case 0x02 -> {if (B != C) { IP = addr;}}
                            case 0x03 -> {if (B != D) { IP = addr;}}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        switch (regB) {
                            case 0x00 -> {if (C != A) { IP = addr;}}
                            case 0x01 -> {if (C != B) { IP = addr;}}
                            case 0x02 -> {}
                            case 0x03 -> {if (C != D) { IP = addr;}}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x00 -> {if (D != A) { IP = addr;}}
                            case 0x01 -> {if (D != B) { IP = addr;}}
                            case 0x02 -> {if (D != C) { IP = addr;}}
                            case 0x03 -> {}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                }
            }
        }
    }

    private void exec_INQ(int opcode) {
        switch (opcode) {
            case 0x2B -> {
                int addr = (get_byte() + (get_byte() << 8));
                int regA = get_byte();
                int regB = get_byte();
                switch (regA) {
                    case 0x00 -> {
                        switch (regB) {
                            case 0x00 -> {}
                            case 0x01 -> {if (A > B) { IP = addr;}}
                            case 0x02 -> {if (A > C) { IP = addr;}}
                            case 0x03 -> {if (A > D) { IP = addr;}}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x00 -> {if (B > A) { IP = addr;}}
                            case 0x01 -> {}
                            case 0x02 -> {if (B > C) { IP = addr;}}
                            case 0x03 -> {if (B > D) { IP = addr;}}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        switch (regB) {
                            case 0x00 -> {if (C > A) { IP = addr;}}
                            case 0x01 -> {if (C > B) { IP = addr;}}
                            case 0x02 -> {}
                            case 0x03 -> {if (C > D) { IP = addr;}}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x00 -> {if (D < A) { IP = addr;}}
                            case 0x01 -> {if (D < B) { IP = addr;}}
                            case 0x02 -> {if (D < C) { IP = addr;}}
                            case 0x03 -> {}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                }
            }
            case 0x2C -> {
                int addr = (get_byte() + (get_byte() << 8));
                int regA = get_byte();
                int regB = get_byte();
                switch (regA) {
                    case 0x00 -> {
                        switch (regB) {
                            case 0x00 -> {}
                            case 0x01 -> {if (A < B) { IP = addr;}}
                            case 0x02 -> {if (A < C) { IP = addr;}}
                            case 0x03 -> {if (A < D) { IP = addr;}}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x00 -> {if (B < A) { IP = addr;}}
                            case 0x01 -> {}
                            case 0x02 -> {if (B < C) { IP = addr;}}
                            case 0x03 -> {if (B < D) { IP = addr;}}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        switch (regB) {
                            case 0x00 -> {if (C < A) { IP = addr;}}
                            case 0x01 -> {if (C < B) { IP = addr;}}
                            case 0x02 -> {}
                            case 0x03 -> {if (C < D) { IP = addr;}}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x00 -> {if (D < A) { IP = addr;}}
                            case 0x01 -> {if (D < B) { IP = addr;}}
                            case 0x02 -> {if (D < C) { IP = addr;}}
                            case 0x03 -> {}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                }
            }
        }
    }

    private void exec_PRN(int opcode) {
        switch (opcode) {
            case 0x2D -> {
                int data = get_byte();
                RAM.write(data, 0xFFFD);
                startPrint();
            }
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
            case 0x2F -> stopPrint();
        }
    }

    private void exec_CAR(int opcode) {
        switch (opcode) {
            case 0x30 -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & 0x10) != 0x00) {
                    IP = addr;
                }
            }
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
            case 0x32 -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & 0x04) != 0x00) {
                    IP = addr;
                }
            }
            case 0x33 -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & 0x04) == 0x00) {
                    IP = addr;
                }
            }
        }
    }

    private void exec_PAR(int opcode) {
        switch (opcode) {
            case 0x34 -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & 0x08) != 0x00) {
                    IP = addr;
                }
            }
            case 0x35 -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & 0x08) == 0x00) {
                    IP = addr;
                }
            }
        }
    }

    private void exec_TMP() {
        int OFFSET = get_byte();
        int DEST = get_byte();
        switch (DEST) {
            case 0x00 -> {A = RAM.read(BP+OFFSET); checkZero(A); checkParity(A);}
            case 0x01 -> {B = RAM.read(BP+OFFSET); checkZero(A); checkParity(A);}
            case 0x02 -> {C = RAM.read(BP+OFFSET); checkZero(A); checkParity(A);}
            case 0x03 -> {D = RAM.read(BP+OFFSET); checkZero(A); checkParity(A);}
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

    private void checkParity(int data) {
        int number_of_bits = 0;
        for (int i = 0; i < 8; i++) {
            int copy = (data << i);
            copy &= 0x01;
            if (copy != 0x00) {
                number_of_bits++;
            }
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
