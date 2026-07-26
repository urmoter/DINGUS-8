using System;

namespace dingus_8;
internal class Memory {
    private readonly byte[] mem = new byte[UInt16.MaxValue];
    
    internal void Write(byte data, ushort address) {
        mem[address] = data;
    }

    internal byte Read(ushort address) => mem[address];

    internal void LoadMemory(FileStream stream) {
        stream.ReadExactly(mem);
    }
}
