import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

public class Cpu {
    private enum OPTYPE {
        NOP,
        MOV,
        MEM,
        MTH,
        STK,
        JMP,
        END,
        OVR,
        STT,
        LOG,
        EQU,
        INQ,
        PRN,
    }

    private final Memory RAM = new Memory();
    private int A = 0;
    private int B = 0;
    private int C = 0;
    private int D = 0;
    private int S = 0;
    private int IP;
    private int SP = 0xFF;

    public Cpu(String bin_file) {
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
        OPTYPE type;

        while (((S & 0x80) == 0) && (IP <= 0xFFFF)) {
            data = get_byte();
            type = determine_opcode_type(data);

            switch (type) {
                case NOP -> {}
                case MOV -> exec_MOV(data);
                case MEM -> exec_MEM(data);
                case MTH -> exec_MTH(data);
                case STK -> exec_STK(data);
                case JMP -> exec_JMP(data);
                case END -> exec_END();
                case OVR -> exec_OVR(data);
                case STT -> exec_STT(data);
                case LOG -> exec_LOG(data);
                case EQU -> exec_EQU(data);
                case INQ -> exec_INQ(data);
                case PRN -> exec_PRN(data);
            }
        }
    }

    private int get_start_address() {
        int LSB = RAM.read(0xFFFE);
        int MSB = RAM.read(0xFFFF);

        return (MSB << 8) + LSB;
    }

    private int get_byte() {
        int data =  RAM.read(IP);
        IP++;
        return data;
    }

    private OPTYPE determine_opcode_type(int opcode) throws RuntimeException {
        if (opcode == 0x00) {
            return OPTYPE.NOP;
        } else if ((opcode > 0x00) && (opcode < 0x06)) {
            return OPTYPE.MOV;
        } else if ((opcode > 0x05) && (opcode < 0x08)) {
            return OPTYPE.MEM;
        } else if ((opcode > 0x07) && (opcode < 0x10)) {
            return OPTYPE.MTH;
        } else if ((opcode > 0x0F) && (opcode < 0x13)) {
            return OPTYPE.STK;
        } else if ((opcode > 0x12) && (opcode < 0x18)) {
            return OPTYPE.JMP;
        } else if (opcode == 0x18) {
            return OPTYPE.END;
        } else if ((opcode > 0x18) && (opcode < 0x1B)) {
            return OPTYPE.OVR;
        } else if ((opcode > 0x1A) && (opcode < 0x25)) {
            return OPTYPE.STT;
        } else if ((opcode > 0x24) && (opcode < 0x29)) {
            return OPTYPE.LOG;
        } else if ((opcode > 0x28) && (opcode < 0x2B)) {
            return OPTYPE.EQU;
        } else if ((opcode > 0x2A) && (opcode < 0x2D)) {
            return OPTYPE.INQ;
        } else if ((opcode > 0x2C) && (opcode < 0x30)) {
            return OPTYPE.PRN;
        } else {
            throw new RuntimeException("INVALID OPCODE!");
        }
    }

    private void exec_MOV(int opcode) {
        switch (opcode) {
            // MOVA
            case 0x01 -> {A = get_byte(); checkZero(A);}
            // MOVB
            case 0x02 -> {B = get_byte(); checkZero(B);}
            // MOVC
            case 0x03 -> {C = get_byte(); checkZero(C);}
            // MOVD
            case 0x04 -> {D = get_byte(); checkZero(D);}
            // CPY
            case 0x05 -> {
                int regA = get_byte();
                int regB = get_byte();

                switch (regA) {
                    case 0x00 -> {
                        switch (regB) {
                            case 0x00 -> checkZero(A);
                            case 0x01 -> {B = A; checkZero(B);}
                            case 0x02 -> {C = A; checkZero(C);}
                            case 0x03 -> {D = A; checkZero(D);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x00 -> {A = B; checkZero(A);}
                            case 0x01 -> checkZero(B);
                            case 0x02 -> {C = B; checkZero(C);}
                            case 0x03 -> {D = B; checkZero(D);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        switch (regB) {
                            case 0x00 -> {A = C; checkZero(A);}
                            case 0x01 -> {B = C; checkZero(B);}
                            case 0x02 -> checkZero(C);
                            case 0x03 -> {D = C; checkZero(D);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x00 -> {A = D; checkZero(A);}
                            case 0x01 -> {B = D; checkZero(B);}
                            case 0x02 -> {C = D; checkZero(C);}
                            case 0x03 -> checkZero(D);
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
                    case 0x00 -> {RAM.write(A, addr); checkZero(RAM.read(addr));}
                    case 0x01 -> {RAM.write(B, addr); checkZero(RAM.read(addr));}
                    case 0x02 -> {RAM.write(C, addr); checkZero(RAM.read(addr));}
                    case 0x03 -> {RAM.write(D, addr); checkZero(RAM.read(addr));}
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }

            case 0x07 -> {
                int addr = (get_byte() + (get_byte() << 8));
                int reg = get_byte();

                switch (reg) {
                    case 0x00 -> {A = RAM.read(addr); checkZero(A);}
                    case 0x01 -> {B = RAM.read(addr); checkZero(B);}
                    case 0x02 -> {C = RAM.read(addr); checkZero(C);}
                    case 0x03 -> {D = RAM.read(addr); checkZero(D);}
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
                            case 0x00 -> {A += A; checkZero(A); checkCarry(A);}
                            case 0x01 -> {A += B; checkZero(A); checkCarry(A);}
                            case 0x02 -> {A += C; checkZero(A); checkCarry(A);}
                            case 0x03 -> {A += D; checkZero(A); checkCarry(A);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x00 -> {B += A; checkZero(B); checkCarry(B);}
                            case 0x01 -> {B += B; checkZero(B); checkCarry(B);}
                            case 0x02 -> {B += C; checkZero(B); checkCarry(B);}
                            case 0x03 -> {B += D; checkZero(B); checkCarry(B);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        switch (regB) {
                            case 0x00 -> {C += A; checkZero(C); checkCarry(C);}
                            case 0x01 -> {C += B; checkZero(C); checkCarry(C);}
                            case 0x02 -> {C += C; checkZero(C); checkCarry(C);}
                            case 0x03 -> {C += D; checkZero(C); checkCarry(C);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x00 -> {D += A; checkZero(D); checkCarry(D);}
                            case 0x01 -> {D += B; checkZero(D); checkCarry(D);}
                            case 0x02 -> {D += C; checkZero(D); checkCarry(D);}
                            case 0x03 -> {D += D; checkZero(D); checkCarry(D);}
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
                            case 0x00 -> {A -= A; checkZero(A); checkNegative(A);}
                            case 0x01 -> {A -= B; checkZero(A); checkNegative(A);}
                            case 0x02 -> {A -= C; checkZero(A); checkNegative(A);}
                            case 0x03 -> {A -= D; checkZero(A); checkNegative(A);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x00 -> {B -= A; checkZero(B); checkNegative(B);}
                            case 0x01 -> {B -= B; checkZero(B); checkNegative(B);}
                            case 0x02 -> {B -= C; checkZero(B); checkNegative(B);}
                            case 0x03 -> {B -= D; checkZero(B); checkNegative(B);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        switch (regB) {
                            case 0x00 -> {C -= A; checkZero(C); checkNegative(C);}
                            case 0x01 -> {C -= B; checkZero(C); checkNegative(C);}
                            case 0x02 -> {C -= C; checkZero(C); checkNegative(C);}
                            case 0x03 -> {C -= D; checkZero(C); checkNegative(C);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x00 -> {D -= A; checkZero(D); checkNegative(D);}
                            case 0x01 -> {D -= B; checkZero(D); checkNegative(D);}
                            case 0x02 -> {D -= C; checkZero(D); checkNegative(D);}
                            case 0x03 -> {D -= D; checkZero(D); checkNegative(D);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
            case 0x0A -> {
                int reg = get_byte();
                switch (reg) {
                    case 0x00 -> {A++; checkZero(A); checkCarry(A);}
                    case 0x01 -> {B++; checkZero(B); checkCarry(B);}
                    case 0x02 -> {C++; checkZero(C); checkCarry(C);}
                    case 0x03 -> {D++; checkZero(D); checkCarry(D);}
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
            case 0x0B -> {
                int reg = get_byte();
                switch (reg) {
                    case 0x00 -> {A--; checkZero(A); checkNegative(A);}
                    case 0x01 -> {B--; checkZero(B); checkNegative(B);}
                    case 0x02 -> {C--; checkZero(C); checkNegative(C);}
                    case 0x03 -> {D--; checkZero(D); checkNegative(D);}
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
            case 0x0C -> {
                int regA = get_byte();
                int regB = get_byte();

                switch (regA) {
                    case 0x00 -> {
                        switch (regB) {
                            case 0x00 -> {A += A + carry; checkZero(A); checkCarry(A);}
                            case 0x01 -> {A += B + carry; checkZero(A); checkCarry(A);}
                            case 0x02 -> {A += C + carry; checkZero(A); checkCarry(A);}
                            case 0x03 -> {A += D + carry; checkZero(A); checkCarry(A);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x00 -> {B += A + carry; checkZero(B); checkCarry(B);}
                            case 0x01 -> {B += B + carry; checkZero(B); checkCarry(B);}
                            case 0x02 -> {B += C + carry; checkZero(B); checkCarry(B);}
                            case 0x03 -> {B += D + carry; checkZero(B); checkCarry(B);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        switch (regB) {
                            case 0x00 -> {C += A + carry; checkZero(C); checkCarry(C);}
                            case 0x01 -> {C += B + carry; checkZero(C); checkCarry(C);}
                            case 0x02 -> {C += C + carry; checkZero(C); checkCarry(C);}
                            case 0x03 -> {C += D + carry; checkZero(C); checkCarry(C);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x00 -> {D += A + carry; checkZero(D); checkCarry(D);}
                            case 0x01 -> {D += B + carry; checkZero(D); checkCarry(D);}
                            case 0x02 -> {D += C + carry; checkZero(D); checkCarry(D);}
                            case 0x03 -> {D += D + carry; checkZero(D); checkCarry(D);}
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
                            case 0x00 -> {A -= (A + negative); checkZero(A); checkNegative(A);}
                            case 0x01 -> {A -= (B + negative); checkZero(A); checkNegative(A);}
                            case 0x02 -> {A -= (C + negative); checkZero(A); checkNegative(A);}
                            case 0x03 -> {A -= (D + negative); checkZero(A); checkNegative(A);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x01 -> {
                        switch (regB) {
                            case 0x00 -> {B -= (A + negative); checkZero(B); checkNegative(B);}
                            case 0x01 -> {B -= (B + negative); checkZero(B); checkNegative(B);}
                            case 0x02 -> {B -= (C + negative); checkZero(B); checkNegative(B);}
                            case 0x03 -> {B -= (D + negative); checkZero(B); checkNegative(B);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x02 -> {
                        switch (regB) {
                            case 0x00 -> {C -= (A + negative); checkZero(C); checkNegative(C);}
                            case 0x01 -> {C -= (B + negative); checkZero(C); checkNegative(C);}
                            case 0x02 -> {C -= (C + negative); checkZero(C); checkNegative(C);}
                            case 0x03 -> {C -= (D + negative); checkZero(C); checkNegative(C);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    case 0x03 -> {
                        switch (regB) {
                            case 0x00 -> {D -= (A + negative); checkZero(D); checkNegative(D);}
                            case 0x01 -> {D -= (B + negative); checkZero(D); checkNegative(D);}
                            case 0x02 -> {D -= (C + negative); checkZero(D); checkNegative(D);}
                            case 0x03 -> {D -= (D + negative); checkZero(D); checkNegative(D);}
                            default -> throw new RuntimeException("INVALID REGISTER");
                        }
                    }
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
            case 0x0E -> {
                int reg = get_byte();
                switch (reg) {
                    case 0x00 -> {A = ~A + 1; checkZero(A); checkNegative(A);}
                    case 0x01 -> {B = ~B + 1; checkZero(B); checkNegative(B);}
                    case 0x02 -> {C = ~C + 1; checkZero(C); checkNegative(C);}
                    case 0x03 -> {D = ~D + 1; checkZero(D); checkNegative(D);}
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }
            case 0x0F -> {
                int reg = get_byte();
                switch (reg) {
                    case 0x00 -> {A = flip_byte((byte) A); checkZero(A); checkNegative(A);}
                    case 0x01 -> {B = flip_byte((byte) B); checkZero(B); checkNegative(B);}
                    case 0x02 -> {C = flip_byte((byte) C); checkZero(C); checkNegative(C);}
                    case 0x03 -> {D = flip_byte((byte) D); checkZero(D); checkNegative(D);}
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
                    case 0x00 -> {A = pop_stack(); checkZero(A);}
                    case 0x01 -> {B = pop_stack(); checkZero(B);}
                    case 0x02 -> {C = pop_stack(); checkZero(C);}
                    case 0x03 -> {D = pop_stack(); checkZero(D);}
                    default -> throw new RuntimeException("INVALID REGISTER");
                }}
        }
    }
    private void exec_JMP(int opcode) {
        switch (opcode) {
            case 0x13 -> IP = (get_byte() + (get_byte() << 8));
            case 0x14 -> {push_stack(IP); IP = (get_byte() + (get_byte() << 8));}
            case 0x15 -> IP = pop_stack();
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
    }
    private void exec_OVR(int opcode) {
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
    private void exec_LOG(int opcode) {}
    private void exec_EQU(int opcode) {}
    private void exec_INQ(int opcode) {}
    private void exec_PRN(int opcode) {}

    private void checkZero(int data) {
        if (data == 0x00) {
            S |= 0x02;
        }
    }
    private void checkNegative(int data) {
        if (data > 0xFF) {
            S |= 0x04;
        }
    }
    private void checkCarry(int data) {
        if (data > 0xFF) {
            S |= 0x10;
        }
    }
    private int flip_byte(byte data) {
        int intSize = 8;
        byte y=0;
        for(int position=intSize-1; position>0; position--){
            y+= (byte) ((data&1)<<position);
            data >>= 1;
        }
        return y;
    }
    private void push_stack(int data) {
        RAM.write(data, SP);
        SP--;
    }
    private int pop_stack() {
        SP++;
        int data = RAM.read(SP);
        RAM.write(0x00, SP);
        return data;
    }
}
