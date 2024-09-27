# Urmoter's Dingus-8, the 8 bit disaster!
Technically 16 bit, but shut up.  
v0.1
---
## What the Dingus-8?

### Overview
The Dingus-8 is an 8-bit microprocessor designed for embedded and low-level computing applications. It employs a 
**little-endian** memory architecture, featuring an **8-bit data bus**, a **16-bit address bus**, and four 
general-purpose **8-bit registers** (A, B, C, and D). The processor can address up to **64KB** of memory and provides a 
simple, yet flexible instruction set for performing data manipulation, control flow, and stack management.

### Key Features
- **8-bit general-purpose registers**: Four registers (A, B, C, D) used for data storage and manipulation.
- **16-bit address bus**: Allows the processor to address memory locations up to **0xFFFF**.
- **Stack Pointer (SP)**: A stack pointer register that manages the stack, mapped from **0x0000 to 0x00FF**.
- **Instruction Pointer (IP)**: A 16-bit register used to store the current memory address of the executing instruction.
- **Status Register (S)**: An 8-bit status register containing several flags for program state and condition monitoring.

### Memory Map
The Dingus-8 provides a well-defined memory layout:
- **0x0000 to 0x00FF**: Stack memory.
- **0x0100 to 0xFFFC**: General-purpose RAM.
- **0xFFFD**: Terminal buffer (used for output operations).
- **0xFFFE to 0xFFFF**: Starting instruction pointer.

### Status Register (S) Flags
The status register consists of 8 bits, each representing a specific condition or flag:

| Bit  |   Name   | Description                                                    |
|:----:|:--------:|----------------------------------------------------------------|
|  0   |  **U**   | User flag, modifiable by the user.                             |
|  1   |  **Z**   | Zero flag, set if the result of an operation is zero.          |
|  2   |  **N**   | Negative flag, set if there's an overflow in subtraction.      |
|  3   |  **P**   | Parity flag, set if an even number of bits are 1.              |
|  4   |  **C**   | Carry flag, set if there's an overflow in addition.            |
|  5   |  **T**   | Terminal flag, active when writing to the terminal buffer.     |
|  6   |  **I**   | Interrupt flag (reserved for future use).                      |
|  7   |  **H**   | Halt flag, set when the processor halts execution.             |


### Instruction Set
The Dingus-8 supports a variety of operations to manage data, control flow, and program execution:

- **Data Movement**:
    - `MOV`: Moves data between registers and memory.
    - `STOR/LODR`: Store and load data from memory.

- **Arithmetic Operations**:
    - `ADDR`: Add two registers.
    - `SUBR`: Subtract one register from another.
    - `INCR/DECR`: Increment or decrement register values.

- **Stack Operations**:
    - `PSHI/PSHR`: Push immediate or register data onto the stack.
    - `POPR`: Pop data from the stack into a register.

- **Control Flow**:
    - `JMP`: Jump to a memory address unconditionally.
    - `CALL/RET`: Call a subroutine and return.
    - `JZ/JNZ`: Jump based on the zero flag.
    - `SETZ/CLRZ`: Set or clear the zero flag.

- **Logical Operations**:
    - `AND/OR/XOR`: Perform bitwise operations between registers.
    - `NOT`: Invert register contents.

- **Output**:
    - `PRNI/PRNR`: Output values to the terminal via the terminal buffer **0xFFFD**.

### Use Case
The Dingus-8 is designed for applications where simplicity and low resource requirements are critical. It is ideal for 
use in embedded systems, educational purposes, or any scenario where lightweight, 
straightforward computation is necessary.

---
# MORE TO COME

---
## This thing was made by Urmoter, no proper copyright yet, whoops.