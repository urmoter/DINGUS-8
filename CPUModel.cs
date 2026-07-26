using System;

namespace dingus_8;

internal enum OpcodeType {
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

public class CPUModel
{
    private readonly Memory mem = new();
    private byte S = 0; // Status Register
    private readonly byte[] genRegisters = new byte[4]; // General Purpose Registers [A,B,C,D]
    private ushort IP; // Instruction Pointer
    private byte SP = 0xFF; // Stack Pointer
    private byte BP = 0xFF; // Base Pointer

    // FLAG MASK CONSTANTS
    private static readonly byte FLAG_L = 0x01; // Less Than
    private static readonly byte FLAG_Z = 0x02; // Zero
    private static readonly byte FLAG_N = 0x04; // Negative
    private static readonly byte FLAG_E = 0x08; // Equal
    private static readonly byte FLAG_C = 0x10; // Carry
    private static readonly byte FLAG_P = 0x20; // Print enable
    private static readonly byte FLAG_G = 0x40; // Greater
    private static readonly byte FLAG_H = 0x80; // Halt/End
    // PRINT LOCATION
    private static readonly ushort PRINT_LOCATION = 0xFFFD;

    private bool IPOverflow = false;

    public CPUModel(FileStream memoryFileStream)
    {
        LoadMemory(memoryFileStream);
        IP = GetStartAddress();

        while (((S & 0x80) == 0x00) && !IPOverflow)
        {
            // Main execution loop
            CheckPrint();
            // If we're on 0xFFFF, then we're gonna wrap-around to 0x00
            IPOverflow = true;
            byte opcode = GetByte();
            ExecuteInstType(opcode, DetermineOpcodeType(opcode));
        }

        Console.WriteLine($"%A: 0x{genRegisters[0]:02X}");
        Console.WriteLine($"%B: 0x{genRegisters[1]:02X}");
        Console.WriteLine($"%C: 0x{genRegisters[2]:02X}");
        Console.WriteLine($"%D: 0x{genRegisters[3]:02X}");
        Console.WriteLine($"@IP: 0x{(IP - 1):04X}");
        Console.WriteLine($"$SP: 0x{SP:02X}");
        Console.WriteLine($"SBP: 0x{BP:02X}");
    }

    private void LoadMemory(FileStream stream) 
    {
        mem.LoadMemory(stream);
    }

    private void CheckPrint()
    {
        if ((S & FLAG_P) != 0)
        {
            Console.Write((char) mem.Read(PRINT_LOCATION));
        }
    }

    /* 
     * Return the starting address of the program in memory
     * which are the last two of bytes in memory.
     * The first byte is the low byte, and the second byte is the high byte.
    */
    private ushort GetStartAddress() => (ushort)(mem.Read(0xFFFE) | (mem.Read(0xFFFF) << 8));

    private byte GetByte()
    {
        byte data = mem.Read(IP);
        IP++;
        return data;
    }

    private static OpcodeType DetermineOpcodeType(byte opcode) => opcode switch 
    {
        0x00 => OpcodeType.NOP,
        0x01 or 0x02 or 0x03 or 0x04 or 0x05 => OpcodeType.MOV,
        0x06 or 0x07 => OpcodeType.MEM,
        0x08 or 0x09 or 0x0A or 0x0B or 0x0C or 0x0D or 0x0F => OpcodeType.MTH,
        0x10 or 0x11 or 0x12 => OpcodeType.STK,
        0x13 or 0x14 or 0x15 or 0x16 or 0x17 => OpcodeType.JMP,
        0x18 => OpcodeType.END,
        0x19 or 0x1A => OpcodeType.LSS,
        0x1B or 0x1C or 0x1D or 0x1E or 0x1F or 0x20 or 0x21 or 0x22 or 0x23 or 0x24 => OpcodeType.STT,
        0x25 or 0x26 or 0x27 or 0x28 => OpcodeType.LOG,
        0x29 or 0x2A => OpcodeType.EQU,
        0x2B or 0x2C => OpcodeType.MSC,
        0x2D or 0x2E or 0x2F => OpcodeType.PRN,
        0x30 or 0x31 => OpcodeType.CAR,
        0x32 or 0x33 => OpcodeType.NEG,
        0x34 or 0x35 => OpcodeType.GRT,
        0x36 => OpcodeType.TMP,
        0x37 => OpcodeType.OFS,
        0x38 or 0x39 or 0x3A or 0x3B or 0x3C or 0x3D or 0x3E or 0x3F or 0x40 or 0x41 => OpcodeType.WID,
        0x42 or 0x43 or 0x44 => OpcodeType.CMP,
        0x45 => OpcodeType.WDC,
        _ => throw new InvalidOperationException($"Invalid opcode: {opcode:X2}"),
    };

    private void ExecuteInstType(byte opcode, OpcodeType type)
    {
        switch (type)
        {
            case OpcodeType.NOP:
                // No operation
                break;
            case OpcodeType.MOV:
                ExecMOV(opcode);
                break;
            case OpcodeType.MEM:
                ExecMEM(opcode);
                break;
            case OpcodeType.MTH:
                ExecMTH(opcode);
                break;
            case OpcodeType.STK:
                ExecSTK(opcode);
                break;
            case OpcodeType.JMP:
                ExecJMP(opcode);
                break;
            case OpcodeType.END:
                ExecEND();
                break;
            case OpcodeType.LSS:
                ExecLSS(opcode);
                break;
            case OpcodeType.STT:
                ExecSTT(opcode);
                break;
            case OpcodeType.LOG:
                ExecLOG(opcode);
                break;
            case OpcodeType.EQU:
                ExecEQU(opcode);
                break;
            case OpcodeType.MSC:
                ExecMSC(opcode);
                break;
            case OpcodeType.PRN:
                ExecPRN(opcode);
                break;
            case OpcodeType.CAR:
                ExecCAR(opcode);
                break;
            case OpcodeType.NEG:
                ExecNEG(opcode);
                break;
            case OpcodeType.GRT:
                ExecGRT(opcode);
                break;
            case OpcodeType.TMP:
                ExecTMP();
                break;
            case OpcodeType.OFS:
                ExecOFS();
                break;
            case OpcodeType.WID:
                ExecWID(opcode);
                break;
            case OpcodeType.CMP:
                ExecCMP(opcode);
                break;
            case OpcodeType.WDC:
                ExecWDC();
                break;
        }
    }

    private void ExecMOV(byte opcode)
    {
        if (opcode < 0x05)
        {
            // MOV[A-D]
            genRegisters[opcode - 0x01] = GetByte();
            CheckZero(genRegisters[opcode - 0x01]);
        }
        else
        {
            // CPY
            byte sourceRegister = GetByte();
            byte destRegister = GetByte();

            genRegisters[destRegister] = genRegisters[sourceRegister];
            CheckZero(genRegisters[destRegister]);
        }
    }

    private void ExecMEM(byte opcode)
    {
        if (opcode == 0x06)
        {
            // STR
            byte sourceRegister = GetByte();
            ushort destAddress = (ushort)(GetByte() | (GetByte() << 8));

            mem.Write(genRegisters[sourceRegister], destAddress);
            CheckZero(genRegisters[sourceRegister]);
        }
        else
        {
            // LDR
            ushort sourceAddress = (ushort)(GetByte() | (GetByte() << 8));
            byte destRegister = GetByte();

            genRegisters[destRegister] = mem.Read(sourceAddress);
            CheckZero(genRegisters[destRegister]);
        }
    }

    private void ExecMTH(byte opcode)
    {
        switch (opcode)
        {
            case 0x08:
                {
                    // ADD
                    byte destRegister = GetByte();
                    byte sourceRegister = GetByte();

                    genRegisters[destRegister] += genRegisters[sourceRegister];

                    CheckZero(genRegisters[destRegister]);
                    CheckCarry(genRegisters[destRegister]);
                    break;
                }

            case 0x09:
                {
                    // SUB
                    byte destRegister = GetByte();
                    byte sourceRegister = GetByte();

                    genRegisters[destRegister] -= genRegisters[sourceRegister];

                    CheckZero(genRegisters[destRegister]);
                    CheckNegative(genRegisters[destRegister]);
                    break;
                }

            case 0x0A:
                {
                    // INC
                    byte destRegister = GetByte();

                    genRegisters[destRegister]++;

                    CheckZero(genRegisters[destRegister]);
                    CheckCarry(genRegisters[destRegister]);
                    break;
                }

            case 0x0B:
                {
                    // DEC
                    byte destRegister = GetByte();

                    genRegisters[destRegister]--;

                    CheckZero(genRegisters[destRegister]);
                    CheckNegative(genRegisters[destRegister]);
                    break;
                }

            case 0x0C:
                {
                    // ADC
                    byte carry = (byte)((S & FLAG_C) != 0x00 ? 1 : 0);
                    byte destRegister = GetByte();
                    byte sourceRegister = GetByte();

                    genRegisters[destRegister] += (byte)(genRegisters[sourceRegister] + carry);

                    CheckZero(genRegisters[destRegister]);
                    CheckCarry(genRegisters[destRegister]);
                    break;
                }

            case 0x0D:
                {
                    // SBB
                    byte negative = (byte)((S & FLAG_N) != 0x00 ? 1 : 0);
                    byte destRegister = GetByte();
                    byte sourceRegister = GetByte();

                    genRegisters[destRegister] -= (byte)(genRegisters[sourceRegister] + negative);

                    CheckZero(genRegisters[destRegister]);
                    CheckNegative(genRegisters[destRegister]);
                    break;
                }

            case 0x0E:
                {
                    // NEG
                    byte destRegister = GetByte();

                    genRegisters[destRegister] = (byte)(~genRegisters[destRegister] + 1);
                    CheckZero(genRegisters[destRegister]);
                    CheckNegative(genRegisters[destRegister]);
                    break;
                }

            case 0x0F:
                {
                    // FLP
                    byte destRegister = GetByte();

                    genRegisters[destRegister] = FlipByte(genRegisters[destRegister]);
                    CheckZero(genRegisters[destRegister]);
                    CheckNegative(genRegisters[destRegister]);
                    break;
                }
        }
    }

    private void ExecSTK(byte opcode)
    {
        switch (opcode)
        {
            case 0x10:
                {
                    // PSHI
                    byte data = GetByte();
                    PushStack(data);
                    break;
                }

            case 0x11:
                {
                    // PSHR
                    byte sourceRegister = GetByte();
                    PushStack(genRegisters[sourceRegister]);
                    break;
                }

            case 0x12:
                {
                    // POP
                    byte destRegister = GetByte();
                    genRegisters[destRegister] = PopStack();
                    CheckZero(genRegisters[destRegister]);
                    break;
                }
        }
    }

    private void ExecJMP(byte opcode)
    {
        switch (opcode)
        {
            case 0x13:
                {
                    // JMP
                    IP = (ushort)(GetByte() | (GetByte() << 8));
                    break;
                }

            case 0x14:
                {
                    // CALL
                    ushort calledAddress = (ushort)(GetByte() | (GetByte() << 8));
                    // Save the return IP
                    PushStack((byte)(IP >> 8));
                    PushStack((byte)(IP));
                    // Set BP to reference function args
                    BP = (byte)(SP + 1);
                    // Now jump to that function
                    IP = calledAddress;
                    break;
                }

            case 0x15:
                {
                    // RET
                    // Return SP to BP incase any local vars were made
                    SP = (byte)(BP - 1);
                    BP = PopStack();
                    // Return to the Callee
                    IP = (byte)(PopStack() | (PopStack() << 8));
                    break;
                }

            case 0x16:
                {
                    // JZ
                    ushort destAddress = (ushort)(GetByte() | (GetByte() << 8));
                    if ((S & FLAG_Z) != 0x00)
                    {
                        IP = destAddress;
                    }
                    break;
                }

            case 0x17:
                {
                    // JNZ
                    ushort destAddress = (ushort)(GetByte() | (GetByte() << 8));
                    if ((S & FLAG_Z) == 0x00)
                    {
                        IP = destAddress;
                    }
                    break;
                }
        }
    }

    private void ExecEND()
    {
        S |= 0x80;
    }

    private void ExecLSS(byte opcode)
    {
        switch (opcode)
        {
            case 0x19:
                {
                    // JL
                    ushort addr = (ushort)(GetByte() | (GetByte() << 8));

                    if ((S & FLAG_L) != 0x00)
                    {
                        IP = addr;
                    }
                    break;
                }

            case 0x1A:
                {
                    // JNL
                    ushort addr = (ushort)(GetByte() | (GetByte() << 8));

                    if ((S & FLAG_L) == 0x00)
                    {
                        IP = addr;
                    }
                    break;
                }
        }
    }

    private void ExecSTT(byte opcode)
    {
        switch (opcode)
        {
            // SETL
            case 0x1B: { S |= FLAG_L; break; }
            // CLRL
            case 0x1C: { S &= (byte)~FLAG_L; break; }
            // SETZ
            case 0x1D: { S |= FLAG_Z; break; }
            // CLRZ
            case 0x1E: { S &= (byte)~FLAG_Z; break; }
            // SETN
            case 0x1F: { S |= FLAG_N; break; }
            // CLRN
            case 0x20: { S &= (byte)~FLAG_N; break; }
            // SETG
            case 0x21: { S |= FLAG_G; break; }
            // CLRG
            case 0x22: { S &= (byte)~FLAG_G; break; }
            // SETC
            case 0x23: { S |= FLAG_C; break; }
            // CLRC
            case 0x24: { S &= (byte)~FLAG_C; break; }
        }
    }

    private void ExecLOG(byte opcode)
    {
    }

    private void ExecEQU(byte opcode)
    {
        switch (opcode)
        {
            case 0x16:
                {
                    // JE
                    ushort destAddress = (ushort)(GetByte() | (GetByte() << 8));
                    if ((S & FLAG_E) != 0x00)
                    {
                        IP = destAddress;
                    }
                    break;
                }

            case 0x17:
                {
                    // JNE
                    ushort destAddress = (ushort)(GetByte() | (GetByte() << 8));
                    if ((S & FLAG_E) == 0x00)
                    {
                        IP = destAddress;
                    }
                    break;
                }
        }
    }

    private void ExecMSC(byte opcode)
    {
    }

    private void ExecPRN(byte opcode)
    {
    }

    private void ExecCAR(byte opcode)
    {
    }

    private void ExecNEG(byte opcode)
    {
    }

    private void ExecGRT(byte opcode)
    {
    }

    private void ExecTMP()
    {
    }

    private void ExecOFS()
    {
    }

    private void ExecWID(byte opcode)
    {
    }

    private void ExecCMP(byte opcode)
    {
    }

    private void ExecWDC()
    {
    }

    private void CheckZero(byte value)
    {
        if (value == 0)
        {
            S |= FLAG_Z;
        }
        else
        {
            S &= (byte)~FLAG_Z;
        }
    }

    private void CheckCarry(byte value)
    {
        if (value > 0xFF)
        {
            S |= FLAG_C;
        }
        else
        {
            S &= (byte)~FLAG_C;
        }
    }

    private void CheckNegative(byte value)
    {
        if ((value & 0x80) != 0)
        {
            S |= FLAG_N;
        }
        else
        {
            S &= (byte)~FLAG_N;
        }
    }

    private static byte FlipByte(byte b)
    {
        return (byte)(
            ((b & 0x80) >> 7) |
            ((b & 0x40) >> 5) |
            ((b & 0x20) >> 3) |
            ((b & 0x10) >> 1) |
            ((b & 0x08) << 1) |
            ((b & 0x04) << 3) |
            ((b & 0x02) << 5) |
            ((b & 0x01) << 7)
        );
    }

    private void PushStack(byte data)
    {
        mem.Write(data, SP);
        SP--;
    }

    private byte PopStack()
    {
        SP++;
        byte data = mem.Read(SP);
        mem.Write(0x00, SP);
        return data;
    }
}
