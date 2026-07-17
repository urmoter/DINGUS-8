import javafx.application.Platform;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

// Merry Christmas, or whatever it is, most of these comments are a retrospective of my sleep-deprived code from 2 A.M.
// I am scared.

/*
    And now, on 2/23/26, it's time to maybe, just maybe, finalize how call works?
    Probably not...
*/

// 4/9/26, it's over..?

/*
    7/17/26, FUCKKKKK, well it's not over! let's make the registers a list to fix those fuckass 4x4 switches :dominictrue:
    Oh and potential C# rewrite, maybe in another branch? :thonk:

    that was quicker than i thought...
 */

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
    WDC,
}

public class Cpu {
    private final Memory RAM = new Memory();
    private int S = 0;
    private int[] registers = new int[4]; // [A, B, C, D]
    private int IP;
    private int SP = 0xFF;
    private int BP = SP;

    // FLAGS
    private static final int FLAG_L = 0x01; // Less
    private static final int FLAG_Z = 0x02; // Zero
    private static final int FLAG_N = 0x04; // Negative
    private static final int FLAG_E = 0x08; // Equal
    private static final int FLAG_C = 0x10; // Carry
    private static final int FLAG_P = 0x20; // Print enable
    private static final int FLAG_G = 0x40; // Greater
    private static final int FLAG_H = 0x80; // Halt/End

    private volatile boolean running = false; // volatile ensures thread visibility
    public void setRunning(boolean running) {this.running = running;}
    public boolean isRunning() {return this.running;}
    public boolean isHalted() {return ((this.S & 0x80) != 0);}

    private java.util.function.Consumer<String> outputConsumer = System.out::print;

    public void setOutputConsumer(java.util.function.Consumer<String> consumer) {
        this.outputConsumer = consumer;
    }

    private final javafx.collections.ObservableList<String> registerStrings =
            javafx.collections.FXCollections.observableArrayList();

    private final javafx.beans.property.SimpleStringProperty currentInstructionWithArgs =
            new javafx.beans.property.SimpleStringProperty("");

    public javafx.beans.property.StringProperty currentInstructionWithArgsProperty() {
        return currentInstructionWithArgs;
    }

    private final javafx.collections.ObservableList<String> stackViewItems =
            javafx.collections.FXCollections.observableArrayList();

    public javafx.collections.ObservableList<String> getStackViewItems() {
        return stackViewItems;
    }

    public void updateStackView() {
        Platform.runLater(() -> {
            stackViewItems.clear();
            int sp = SP;
            int bp = BP;

            int start = Math.max(0, sp - 8);
            int end = Math.min(0xFF, sp + 7);

            for (int addr = end; addr >= start; addr--) {
                int value = RAM.read(addr);
                String entry = String.format("0x%02X: 0x%02X", addr, value);
                entry += (addr == sp) ? " <- SP" : "";
                entry += (addr == bp) ? " <- BP" : "";
                stackViewItems.add(entry);
            }
        });
    }

    public javafx.collections.ObservableList<String> getRegisterStrings() {
        updateRegisterStrings();
        return registerStrings;
    }

    private void updateRegisterStrings() {
        // Update the ObservableList with all 8 registers
        Platform.runLater(() -> {
            registerStrings.setAll(
                    String.format("A: 0x%02X", registers[0]),
                    String.format("B: 0x%02X", registers[1]),
                    String.format("C: 0x%02X", registers[2]),
                    String.format("D: 0x%02X", registers[3]),
                    String.format("IP: 0x%04X", IP),
                    String.format("SP: 0x%02X", SP),
                    String.format("BP: 0x%02X", BP)
            );
        });
    }

    // Call this inside step()
    public void flushRegisters() {
        updateRegisterStrings();
    }

    private final javafx.beans.property.StringProperty[] flagProperties =
            new javafx.beans.property.StringProperty[8];
    public javafx.beans.property.StringProperty getFlagProperty(int index) {
        return flagProperties[index];
    }
    public void updateFlagsView() {
        flagProperties[0].set(((S & FLAG_L) != 0) ? "1" : "0");
        flagProperties[1].set(((S & FLAG_Z) != 0) ? "1" : "0");
        flagProperties[2].set(((S & FLAG_N) != 0) ? "1" : "0");
        flagProperties[3].set(((S & FLAG_E) != 0) ? "1" : "0");
        flagProperties[4].set(((S & FLAG_C) != 0) ? "1" : "0");
        flagProperties[5].set(((S & FLAG_P) != 0) ? "1" : "0");
        flagProperties[6].set(((S & FLAG_G) != 0) ? "1" : "0");
        flagProperties[7].set(((S & FLAG_H) != 0) ? "1" : "0");
    }

    private void loadBinary(String bin_file) {
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
    }

    public Cpu(String bin_file) {
        loadBinary(bin_file);
        for (int i = 0; i < 8; i++) {
            flagProperties[i] = new javafx.beans.property.SimpleStringProperty("0");
        }
        // Let's start this shit!
        IP = get_start_address();
    }

    public void step() {
        if ((S & FLAG_H) != 0 || IP > 0xFFFF) return;
        String instrWithArgs = getCurrentInstruction(); // format args correctly

        // Update GUI
        currentInstructionWithArgs.set(instrWithArgs);

        // Now actually execute
        int opcode = get_byte();
        type_exec(opcode, determine_opcode_type(opcode));
        flushRegisters();
        updateStackView();
        updateFlagsView();

        // handle terminal output live
        if ((S & FLAG_P) != 0) {
            int value = RAM.read(0xFFFD);
            outputConsumer.accept(Character.toString((char)value));
        }


    }

    public String getCurrentInstruction() {
        int opcode = peek_byte(0);
        String name = getInstructionName(opcode);

        return switch (opcode) {
            case 0x00, 0x15, 0x18, 0x2F -> name; // single-byte no-arg

            case 0x01, 0x02, 0x03, 0x04 -> String.format("%s $%02X", name, peek_byte(1)); // MOV immediate

            case 0x0A, 0x0B, 0x0E, 0x0F -> String.format("%s %%%s", name, regName(peek_byte(1))); // INC/DEC/NEG/FLP

            case 0x05, 0x0C, 0x0D, 0x25, 0x26, 0x27 ->
                    String.format("%s %%%s %%%s", name, regName(peek_byte(1)), regName(peek_byte(2))); // REG8 -> REG8

            case 0x10 -> String.format("%s $%02X", name, peek_byte(1)); // PSHI
            case 0x11, 0x12 -> String.format("%s %%%s", name, regName(peek_byte(1))); // PSHR / POP

            case 0x06 -> String.format("%s %%%s @%04X", name, regName(peek_byte(1)), peek_word(2)); // STR
            case 0x07 -> String.format("%s @%04X %%%s", name, peek_word(1), regName(peek_byte(3))); // LDR

            case 0x13, 0x14, 0x16, 0x17, 0x19, 0x1A, 0x29, 0x2A, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35 ->
                    String.format("%s @%04X", name, peek_word(1)); // jumps

            case 0x2C -> String.format("%s %%%s %%%s %%%s", name, regName(peek_byte(1)), regName(peek_byte(2)), regName(peek_byte(3))); // LEA

            case 0x2E -> String.format("%s %%%s", name, regName(peek_byte(1))); // PRNR

            case 0x37 -> String.format("%s @%04X %%%s %%%s", name, peek_word(1), regName(peek_byte(3)), regName(peek_byte(4))); // LDRO

            case 0x38, 0x3B, 0x3C, 0x3F -> String.format("%s %%%s @%04X", name, regName(peek_byte(1)), peek_word(2)); // wide ops

            case 0x40, 0x41 -> String.format("%s %%%s", name, regName(peek_byte(1))); // INCW / DECW

            case 0x42 -> String.format("%s %%%s %%%s", name, regName(peek_byte(1)), regName(peek_byte(2))); // CMP

            case 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x43, 0x44 -> name; // flags

            default -> String.format("0x%02X ???", opcode); // fallback
        };
    }

    // Helper for REG8
    private String regName(int reg) {
        return switch (reg) {
            case 0x00 -> "A";
            case 0x01 -> "B";
            case 0x02 -> "C";
            case 0x03 -> "D";
            default -> "UNK";
        };
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
            case WDC -> exec_WDC();
        }
    }

    private int get_start_address() {
        // return the combined values for [0xFFFF] + [0xFFFE]
        int LSB = RAM.read(0xFFFE);
        int MSB = RAM.read(0xFFFF);
        // Debug for when shit hits the fan
        // System.out.printf("Start: 0x%04X%n", (MSB << 8) | LSB);
        return (MSB << 8) | LSB;
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
            case 0x45 -> OpcodeType.WDC;
            default -> throw new RuntimeException("INVALID OPCODE " + String.format("0x%02X", opcode) + "!");
        };
    }

    private void exec_MOV(int opcode) {
        if (opcode < 0x05) {
            // MOV(A-D)
            registers[opcode - 1] = get_byte();
            checkZero(registers[opcode - 1]);
        } else {
            // CPY
            int regAindex = get_byte();
            int regBindex = get_byte();
            registers[regBindex] = registers[regAindex];
            checkZero(registers[regBindex]);
        }
    }

    private void exec_MEM(int opcode) {
        if (opcode == 6) {
            // STR
            int regIndex = get_byte();
            int addr = (get_byte() + (get_byte() << 8));

            RAM.write(registers[regIndex], addr);
            checkZero(registers[regIndex]);
        } else {
            int addr = (get_byte() + (get_byte() << 8));
            int regIndex = get_byte();

            registers[regIndex] = RAM.read(addr);
            checkZero(registers[regIndex]);
        }
    }

    private void exec_MTH(int opcode) {
        switch (opcode) {
            // ADD
            case 0x08 -> {
                int regAindex = get_byte();
                int regBindex = get_byte();

                registers[regAindex] += registers[regBindex];
                checkZero(registers[regAindex]);
                checkCarry(registers[regAindex]);
                registers[regAindex] &= 0xFF;
            }
            // SUB
            case 0x09 -> {
                int regAindex = get_byte();
                int regBindex = get_byte();

                registers[regAindex] -= registers[regBindex];
                checkZero(registers[regAindex]);
                checkNegative(registers[regAindex]);
                registers[regAindex] &= 0xFF;
            }
            // INC
            case 0x0A -> {
                int regIndex = get_byte();
                registers[regIndex]++;
                checkZero(registers[regIndex]);
                checkCarry(registers[regIndex]);
                registers[regIndex] &= 0xFF;
            }
            // DEC
            case 0x0B -> {
                int regIndex = get_byte();
                registers[regIndex]--;
                checkZero(registers[regIndex]);
                checkNegative(registers[regIndex]);
                registers[regIndex] &= 0xFF;
            }
            // ADC
            case 0x0C -> {
                int carry = ((S & 0x10) != 0x00) ? 1 : 0;
                int regAindex = get_byte();
                int regBindex = get_byte();

                registers[regAindex] += (registers[regAindex] + carry);
                checkZero(registers[regAindex]);
                checkCarry(registers[regAindex]);
                registers[regAindex] &= 0xFF;
            }
            // SBB
            case 0x0D -> {
                int negative = ((S & 0x04) != 0x00) ? 1 : 0;
                int regAindex = get_byte();
                int regBindex = get_byte();

                registers[regAindex] -= (registers[regAindex] + negative);
                checkZero(registers[regAindex]);
                checkNegative(registers[regAindex]);
                registers[regAindex] &= 0xFF;
            }
            // NEG
            case 0x0E -> {
                int regIndex = get_byte();

                registers[regIndex] = (~registers[regIndex] + 1);
                checkZero(registers[regIndex]);
                checkNegative(registers[regIndex]);
                registers[regIndex] &= 0xFF;
            }
            // FLP
            case 0x0F -> {
                int regIndex = get_byte();

                registers[regIndex] = flip_byte((byte) registers[regIndex]);
                checkZero(registers[regIndex]);
                checkNegative(registers[regIndex]);
                registers[regIndex] &= 0xFF;
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
                int regIndex = get_byte();
                push_stack(registers[regIndex]);
            }
            // POP
            case 0x12 -> {
                int regIndex = get_byte();
                registers[regIndex] = pop_stack();
                checkZero(registers[regIndex]);
            }
        }
    }

    private void exec_JMP(int opcode) {
        switch (opcode) {
            // JMP
            case 0x13 -> IP = (get_byte() + (get_byte() << 8));
            // CALL
            case 0x14 -> {
                int dest = (get_byte() + (get_byte() << 8));
                // Save next instruction
                push_stack((byte) (IP>> 8));
                push_stack((byte) IP);
                push_stack(BP);
                // Set base pointer for Arg reference
                BP = (SP+1);
                // Get to the called code
                IP = dest;
            }
            // RET
            case 0x15 -> {
                // Set SP back to BP (in case any local variables were made)
                SP = (BP-1);
                BP = pop_stack();
                int LSB = pop_stack();
                int MSB = pop_stack();
                IP = (MSB<< 8) | LSB;
            }
            // JZ
            case 0x16 -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & FLAG_Z) != 0x00) {
                    IP = addr;
                }
            }
            // JNZ
            case 0x17 -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & FLAG_Z) == 0x00) {
                    IP = addr;
                }
            }
        }
    }

    private void exec_END() {
        // END
        S |= 0x80;
        System.out.println("%A: " + String.format("0x%02X", registers[0]));
        System.out.println("%B: " + String.format("0x%02X", registers[1]));
        System.out.println("%C: " + String.format("0x%02X", registers[2]));
        System.out.println("%D: " + String.format("0x%02X", registers[3]));
        System.out.println("%S: " + String.format("0x%02X", S));
        System.out.println("@IP: " + String.format("0x%04X", (IP - 1)));
        System.out.println("%SP: " + String.format("0x%02X", SP));
        System.out.println("%BP: " + String.format("0x%02X", BP));
        DEBUG_PRINT_STACK_WITH_POINTERS(SP,BP);
    }

    private void exec_LSS(int opcode) {
        switch (opcode) {
            // JL
            case 0x19 -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & FLAG_L) != 0x00) {
                    IP = addr;
                }
            }
            // JNL
            case 0x1A -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & FLAG_L) == 0x00) {
                    IP = addr;
                }
            }
        }
    }

    private void exec_STT(int opcode) {
        switch (opcode) {
            // SETL
            case 0x1B -> S |= FLAG_L;
            // CLRL
            case 0x1C -> S &= ~FLAG_L;
            // SETZ
            case 0x1D -> S |= FLAG_Z;
            // CLRZ
            case 0x1E -> S &= ~FLAG_Z;
            // SETN
            case 0x1F -> S |= FLAG_N;
            // CLRN
            case 0x20 -> S &= ~FLAG_N;
            // SETG
            case 0x21 -> S |= FLAG_G;
            // CLRG
            case 0x22 -> S &= ~FLAG_G;
            // SETC
            case 0x23 -> S |= FLAG_C;
            // CLRC
            case 0x24 -> S &= ~FLAG_C;
        }
    }

    private void exec_LOG(int opcode) {
        switch (opcode) {
            // AND
            case 0x25 -> {
                int regAindex = get_byte();
                int regBindex = get_byte();
                registers[regAindex] &= registers[regBindex];
                checkZero(registers[regAindex]);
            }
            // OR
            case 0x26 -> {
                int regAindex = get_byte();
                int regBindex = get_byte();
                registers[regAindex] |= registers[regBindex];
                checkZero(registers[regAindex]);
            }
            // XOR
            case 0x27 -> {
                int regAindex = get_byte();
                int regBindex = get_byte();
                registers[regAindex] ^= registers[regBindex];
                checkZero(registers[regAindex]);
            }
            // NOT
            case 0x28 -> {
                int regAindex = get_byte();
                registers[regAindex] = ~registers[regAindex];
                checkZero(registers[regAindex]);
            }
        }
    }

    private void exec_EQU(int opcode) {
        switch (opcode) {
            // JE
            case 0x29 -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & FLAG_E) != 0x00) {
                    IP = addr;
                }
            }
            // JNE
            case 0x2A -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & FLAG_E) == 0x00) {
                    IP = addr;
                }
            }
        }
    }

    private void exec_MSC(int opcode) {
        switch (opcode) {
            // CLR
            case 0x2B -> {
                int regindex = get_byte();

                registers[regindex] = 0;
                checkZero(registers[regindex]);
            }
            // LEA
            case 0x2C -> {
                int regPair = get_byte();   // 0x04 or 0x05 (wAB, wCD)
                int regOffsetIndex = get_byte(); // 0x00-0x03 (A-D) used as offset
                int regDestIndex = get_byte();   // 0x00-0x03 (A-D) destination

                switch (regPair) {
                    case 0x04 -> { // wAB
                        int base = ((registers[0] << 8) | registers[1]);
                        int addr = (base + registers[regOffsetIndex]);
                        registers[regDestIndex] = RAM.read(addr);
                        checkZero(registers[regDestIndex]);
                    }
                    case 0x05 -> {
                        int base = ((registers[2] << 8) | registers[3]);
                        int addr = (base + registers[regOffsetIndex]);
                        registers[regDestIndex] = RAM.read(addr);
                        checkZero(registers[regDestIndex]);
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
                int regIndex = get_byte();
                RAM.write(registers[regIndex], 0xFFFD);
                startPrint();
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
                if ((S & FLAG_C) != 0x00) {
                    IP = addr;
                }
            }
            // JNC
            case 0x31 -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & FLAG_C) == 0x00) {
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
                if ((S & FLAG_N) != 0x00) {
                    IP = addr;
                }
            }
            // JNN
            case 0x33 -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & FLAG_N) == 0x00) {
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
                if ((S & FLAG_G) != 0x00) {
                    IP = addr;
                }
            }
            // JNG
            case 0x35 -> {
                int addr = (get_byte() + (get_byte() << 8));
                if ((S & FLAG_G) == 0x00) {
                    IP = addr;
                }
            }
        }
    }

    private void exec_TMP() {
        // IBPR
        int OFFSET = get_byte();
        int DESTIndex = get_byte();

        registers[DESTIndex] = RAM.read(BP+OFFSET);
        checkZero(registers[DESTIndex]);
    }

    private void exec_OFS() {
        int addr = (get_byte() + (get_byte() << 8));
        int OFFSETIndex = get_byte();
        int destIndex = get_byte();

        registers[destIndex] = RAM.read(addr + registers[OFFSETIndex]);
        checkZero(registers[destIndex]);
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
                        registers[0] = hi; registers[1] = lo;
                    }
                    case 0x05 -> {
                        registers[2] = hi; registers[3] = lo;
                    }
                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }

            // PSHW
            case 0x39 -> {
                int reg = get_byte();

                switch (reg) {
                    case 0x04 -> {
                        push_stack(registers[0]);
                        push_stack(registers[1]);
                    }
                    case 0x05 -> {
                        push_stack(registers[2]);
                        push_stack(registers[3]);
                    }

                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }

            // POPW
            case 0x3A -> {
                int reg = get_byte();

                switch (reg) {
                    case 0x04 -> {
                        registers[1] = pop_stack();
                        registers[0] = pop_stack();
                    }
                    case 0x05 -> {
                        registers[2] = pop_stack();
                        registers[3] = pop_stack();
                    }

                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }

            // LDW
            case 0x3B -> {
                int regA = get_byte();
                int regBIndex = get_byte();

                switch (regA) {
                    case 0x04 -> {
                        registers[regBIndex] = RAM.read((registers[0]<<8) | registers[1]);
                    }
                    case 0x05 -> {
                        registers[regBIndex] = RAM.read((registers[2]<<8) | registers[3]);
                    }

                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }

            // STW
            case 0x3C -> {
                int regAIndex = get_byte();
                int regB = get_byte();

                switch (regB) {
                    case 0x04 -> {
                        RAM.write(registers[regAIndex], (registers[0]<<8) | registers[1]);
                    }
                    case 0x05 -> {
                        RAM.write(registers[regAIndex], (registers[2]<<8) | registers[3]);
                    }

                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }

            // JMPW
            case 0x3D -> {
                int reg = get_byte();

                switch (reg) {
                    case 0x04 -> {
                        IP = (registers[0]<<8) | registers[1];
                    }
                    case 0x05 -> {
                        IP = (registers[2]<<8) | registers[3];
                    }

                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }

            // CALW
            case 0x3E -> {
                int reg = get_byte();
                switch (reg) {
                    case 0x04 -> {
                        int dest = (registers[0]<<8) | registers[1];
                        // Push IP and BP
                        push_stack((byte) (IP>> 8));
                        push_stack((byte) IP);
                        push_stack(BP);
                        // Set base pointer for Arg reference
                        BP = SP;
                        // Get to the called code
                        IP = dest;
                    }
                    case 0x05 -> {
                        int dest = (registers[2]<<8) | registers[3];
                        // Push IP and BP
                        push_stack((byte) (IP>> 8));
                        push_stack((byte) IP);
                        push_stack(BP);
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
                        int temp_16 = (registers[0]<<8) | registers[1];
                        temp_16 += imm;
                        registers[0] = temp_16>>8;
                        registers[1] = temp_16;

                        registers[0] &= 0xFF;
                        registers[1] &= 0xFF;

                    }
                    case 0x05 -> {
                        int temp_16 = (registers[2]<<8) | registers[3];
                        temp_16 += imm;
                        registers[2] = temp_16>>8;
                        registers[3] = temp_16;

                        registers[2] &= 0xFF;
                        registers[3] &= 0xFF;
                    }

                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }

            // INCW
            case 0x40 -> {
                int reg = get_byte();

                switch (reg) {
                    case 0x04 -> {
                        int temp_16 = (registers[0]<<8) | registers[1];
                        temp_16++;
                        registers[0] = temp_16>>8;
                        registers[1] = temp_16;

                        registers[0] &= 0xFF;
                        registers[1] &= 0xFF;

                    }
                    case 0x05 -> {
                        int temp_16 = (registers[2]<<8) | registers[3];
                        temp_16++;
                        registers[2] = temp_16>>8;
                        registers[3] = temp_16;

                        registers[2] &= 0xFF;
                        registers[3] &= 0xFF;
                    }

                    default -> throw new RuntimeException("INVALID REGISTER");
                }
            }

            // DECW
            case 0x41 -> {
                int reg = get_byte();

                switch (reg) {
                    case 0x04 -> {
                        int temp_16 = (registers[0]<<8) | registers[1];
                        temp_16--;
                        registers[0] = temp_16>>8;
                        registers[1] = temp_16;

                        registers[0] &= 0xFF;
                        registers[1] &= 0xFF;

                    }
                    case 0x05 -> {
                        int temp_16 = (registers[2]<<8) | registers[3];
                        temp_16--;
                        registers[2] = temp_16>>8;
                        registers[3] = temp_16;

                        registers[2] &= 0xFF;
                        registers[3] &= 0xFF;
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
                int regAIndex = get_byte();
                int regBIndex = get_byte();

                if (registers[regAIndex] == registers[regBIndex]) {
                    S |= FLAG_E; // E
                    S &= ~FLAG_L; // L
                    S &= ~FLAG_G; // G
                } else if (registers[regAIndex] < registers[regBIndex]) {
                    S &= ~FLAG_E; // E
                    S |= FLAG_L; // L
                    S &= ~FLAG_G; // G
                } else {
                    S &= ~FLAG_E; // E
                    S &= ~FLAG_L; // L
                    S |= FLAG_G; // G
                }
            }

            // SETG
            case 0x43 -> S |= 0x40;

            // CLRG
            case 0x44 -> S &= 0x40;
        }
    }

    private void exec_WDC() {
        int OFFSET = get_byte();
        int DEST = get_byte();
        switch (DEST) {
            case 0x04 -> {
                registers[0] = RAM.read(BP+OFFSET+1);
                registers[1] = RAM.read(BP+OFFSET);
            }
            case 0x05 -> {
                registers[2] = RAM.read(BP+OFFSET+1);
                registers[3] = RAM.read(BP+OFFSET);
            }
            default -> throw new RuntimeException("INVALID REGISTER");
        }
    }

    private void checkZero(int data) {
        if (data == 0x00) {
            S |= FLAG_Z;
        } else {
            S &= ~FLAG_Z;
        }
    }

    private void checkNegative(int data) {
        if ((data & 0x80) != 0) {   // check sign bit of 8-bit value
            S |= 0x04;
        } else {
            S &= 0xFB;
        }
    }

    private void checkCarry(int data) {
        if (data > 0xFF) {
            S |= FLAG_C;
        } else {
            S &= ~FLAG_C;
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
        S |= FLAG_P;
    }

    private void stopPrint() {
        S &= ~FLAG_P;
    }

    private int pop_stack() {
        SP++;
        int data = RAM.read(SP);
        RAM.write(0x00, SP);
        return data;
    }

    private void DEBUG_PRINT_STACK_WITH_POINTERS(int SP, int BP) {
        for (int i = 0xFF; i > SP; i--) {
            System.out.println("+----------+");
            System.out.println("+---" + String.format("0x%02X", RAM.read(i)) + "---+ <-- SP+" + (i-SP) + ((BP==i)?" & BP":""));
        }
        System.out.println("+----------+");
        System.out.println("+---NULL---+ <-- SP+0" + ((BP==SP)?" & BP":""));
        System.out.println("+----------+");
    }

    public String getInstructionName(int opcode) {
        return switch (opcode) {
            case 0x00 -> "NOP";

            // MOV / CPY
            case 0x01 -> "MOVA";
            case 0x02 -> "MOVB";
            case 0x03 -> "MOVC";
            case 0x04 -> "MOVD";
            case 0x05 -> "CPY";

            // MEMORY
            case 0x06 -> "STR";
            case 0x07 -> "LDR";

            // MATH
            case 0x08 -> "ADD";
            case 0x09 -> "SUB";
            case 0x0A -> "INC";
            case 0x0B -> "DEC";
            case 0x0C -> "ADC";
            case 0x0D -> "SBB";
            case 0x0E -> "NEG";
            case 0x0F -> "FLP";

            // STACK
            case 0x10 -> "PSHI";
            case 0x11 -> "PSHR";
            case 0x12 -> "POP";

            // JUMP / CALLS
            case 0x13 -> "JMP";
            case 0x14 -> "CALL";
            case 0x15 -> "RET";
            case 0x16 -> "JZ";
            case 0x17 -> "JNZ";

            // HALT
            case 0x18 -> "HLT";

            // LESS
            case 0x19 -> "JL";
            case 0x1A -> "JNL";

            // STATUS FLAGS
            case 0x1B -> "SETL";
            case 0x1C -> "CLRL";
            case 0x1D -> "SETZ";
            case 0x1E -> "CLRZ";
            case 0x1F -> "SETN";
            case 0x20 -> "CLRN";
            case 0x21 -> "SETE";
            case 0x22 -> "CLRE";
            case 0x23 -> "SETC";
            case 0x24 -> "CLRC";

            // LOGIC
            case 0x25 -> "AND";
            case 0x26 -> "OR";
            case 0x27 -> "XOR";
            case 0x28 -> "NOT";

            // EQUAL / CONDITIONAL
            case 0x29 -> "JE";
            case 0x2A -> "JNE";

            // LEA
            case 0x2C -> "LEA";

            // PRINT
            case 0x2D -> "PRNI";
            case 0x2E -> "PRNR";
            case 0x2F -> "ENPR";

            // CARRY CONDITIONAL
            case 0x30 -> "JC";
            case 0x31 -> "JNC";

            // NEGATIVE CONDITIONAL
            case 0x32 -> "JN";
            case 0x33 -> "JNN";

            // GREATER CONDITIONAL
            case 0x34 -> "JG";
            case 0x35 -> "JNGS";

            // BP RELATIVE
            case 0x36 -> "IBPR";

            // LOAD WITH OFFSET
            case 0x37 -> "LDRO";

            // WIDE (16-bit) OPERATIONS
            case 0x38 -> "MOVW";
            case 0x39 -> "PSHW";
            case 0x3A -> "POPW";
            case 0x3B -> "LDW";
            case 0x3C -> "STW";
            case 0x3D -> "JMPW";
            case 0x3E -> "CALW";
            case 0x3F -> "ADDW";
            case 0x40 -> "INCW";
            case 0x41 -> "DECW";

            // COMPARE / FLAGS
            case 0x42 -> "CMP";
            case 0x43 -> "SETG";
            case 0x44 -> "CLRG";

            // CONTINUED WIDE OPERATIONS
            case 0x45 -> "IBPW";

            default -> "UNKNOWN";
        };
    }

    private int peek_byte() {
        return RAM.read(IP); // read memory at current IP
    }

    // Peek byte with offset
    private int peek_byte(int offset) {
        return RAM.read(IP + offset);
    }

    // Peek 16-bit little endian
    private int peek_word(int offset) {
        int low = peek_byte(offset);
        int high = peek_byte(offset + 1);
        return (high << 8) | low;
    }
}
