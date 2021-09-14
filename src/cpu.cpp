/*
 * This file is part of the MOS6502 emulator.
 * (https://github.com/ericwoude/MOS6502)
 *
 * The MIT License (MIT)
 *
 * Copyright © 2021 Eric van der Woude
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the “Software”), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "cpu.h"

#include <iomanip>
#include <sstream>

#define ADD_DISPATCH(HEX, NAME, CYCLES, ADDRESSING_MODE) \
    instruction.addr = &CPU::Addr##ADDRESSING_MODE;      \
    instruction.op = &CPU::Op##NAME;                     \
    instruction.cycles = CYCLES;                         \
    dispatch_table[HEX] = instruction

CPU::CPU()
{
    // Prefill dispatch table with illegal opcode handlers
    Instruction instruction;
    instruction.addr = &CPU::AddrOpcode;
    instruction.op = &CPU::OpIllegal;
    instruction.cycles = 0;
    dispatch_table.fill(instruction);

    // LOAD & STORE
    ADD_DISPATCH(0xA9, LDA, 2, Immediate);
    ADD_DISPATCH(0xA5, LDA, 3, ZeroPage);
    ADD_DISPATCH(0xB5, LDA, 4, ZeroPageX);
    ADD_DISPATCH(0xAD, LDA, 4, Absolute);
    ADD_DISPATCH(0xBD, LDA, 4, AbsoluteX);
    ADD_DISPATCH(0xB9, LDA, 4, AbsoluteY);
    ADD_DISPATCH(0xA1, LDA, 6, IndexedIndirect);
    ADD_DISPATCH(0xB1, LDA, 5, IndirectIndexed);

    ADD_DISPATCH(0xA2, LDX, 2, Immediate);
    ADD_DISPATCH(0xA6, LDX, 3, ZeroPage);
    ADD_DISPATCH(0xB6, LDX, 4, ZeroPageY);
    ADD_DISPATCH(0xAE, LDX, 4, Absolute);
    ADD_DISPATCH(0xBE, LDX, 4, AbsoluteY);

    ADD_DISPATCH(0xA0, LDY, 2, Immediate);
    ADD_DISPATCH(0xA4, LDY, 3, ZeroPage);
    ADD_DISPATCH(0xB4, LDY, 4, ZeroPageX);
    ADD_DISPATCH(0xAC, LDY, 4, Absolute);
    ADD_DISPATCH(0xBC, LDY, 4, AbsoluteX);

    ADD_DISPATCH(0x85, STA, 3, ZeroPage);
    ADD_DISPATCH(0x95, STA, 4, ZeroPageX);
    ADD_DISPATCH(0x8D, STA, 4, Absolute);
    ADD_DISPATCH(0x9D, STA, 5, AbsoluteX5);
    ADD_DISPATCH(0x99, STA, 5, AbsoluteY5);
    ADD_DISPATCH(0x81, STA, 6, IndexedIndirect);
    ADD_DISPATCH(0x91, STA, 6, IndirectIndexed6);

    ADD_DISPATCH(0x86, STX, 3, ZeroPage);
    ADD_DISPATCH(0x96, STX, 4, ZeroPageY);
    ADD_DISPATCH(0x8e, STX, 4, Absolute);

    ADD_DISPATCH(0x84, STY, 3, ZeroPage);
    ADD_DISPATCH(0x94, STY, 4, ZeroPageX);
    ADD_DISPATCH(0x8C, STY, 4, Absolute);

    // REGISTER TRANSFERS
    ADD_DISPATCH(0xAA, TAX, 2, Implied);
    ADD_DISPATCH(0xA8, TAY, 2, Implied);
    ADD_DISPATCH(0x8A, TXA, 2, Implied);
    ADD_DISPATCH(0x98, TYA, 2, Implied);

    // STACK OPERATIONS
    ADD_DISPATCH(0xBA, TSX, 2, Implied);
    ADD_DISPATCH(0x9A, TXS, 2, Implied);
    ADD_DISPATCH(0x48, PHA, 3, Implied);
    ADD_DISPATCH(0x08, PHP, 3, Implied);
    ADD_DISPATCH(0x68, PLA, 4, Implied);
    ADD_DISPATCH(0x28, PLP, 4, Implied);

    // LOGICAL OPERATIONS
    ADD_DISPATCH(0x29, AND, 2, Immediate);
    ADD_DISPATCH(0x25, AND, 3, ZeroPage);
    ADD_DISPATCH(0x35, AND, 4, ZeroPageX);
    ADD_DISPATCH(0x2D, AND, 4, Absolute);
    ADD_DISPATCH(0x3D, AND, 4, AbsoluteX);
    ADD_DISPATCH(0x39, AND, 4, AbsoluteY);
    ADD_DISPATCH(0x21, AND, 6, IndexedIndirect);
    ADD_DISPATCH(0x31, AND, 5, IndirectIndexed);

    ADD_DISPATCH(0x49, EOR, 2, Immediate);
    ADD_DISPATCH(0x45, EOR, 3, ZeroPage);
    ADD_DISPATCH(0x55, EOR, 4, ZeroPageX);
    ADD_DISPATCH(0x4D, EOR, 4, Absolute);
    ADD_DISPATCH(0x5D, EOR, 4, AbsoluteX);
    ADD_DISPATCH(0x59, EOR, 4, AbsoluteY);
    ADD_DISPATCH(0x41, EOR, 6, IndexedIndirect);
    ADD_DISPATCH(0x51, EOR, 5, IndirectIndexed);

    ADD_DISPATCH(0x09, ORA, 2, Immediate);
    ADD_DISPATCH(0x05, ORA, 3, ZeroPage);
    ADD_DISPATCH(0x15, ORA, 4, ZeroPageX);
    ADD_DISPATCH(0x0D, ORA, 4, Absolute);
    ADD_DISPATCH(0x1D, ORA, 4, AbsoluteX);
    ADD_DISPATCH(0x19, ORA, 4, AbsoluteY);
    ADD_DISPATCH(0x01, ORA, 6, IndexedIndirect);
    ADD_DISPATCH(0x11, ORA, 5, IndirectIndexed);

    ADD_DISPATCH(0x24, BIT, 3, ZeroPage);
    ADD_DISPATCH(0x2C, BIT, 4, Absolute);

    // ARITHMETIC OPERATIONS
    ADD_DISPATCH(0x69, ADC, 2, Immediate);
    ADD_DISPATCH(0x65, ADC, 3, ZeroPage);
    ADD_DISPATCH(0x75, ADC, 4, ZeroPageX);
    ADD_DISPATCH(0x6D, ADC, 4, Absolute);
    ADD_DISPATCH(0x7D, ADC, 4, AbsoluteX);
    ADD_DISPATCH(0x79, ADC, 4, AbsoluteY);
    ADD_DISPATCH(0x61, ADC, 6, IndexedIndirect);
    ADD_DISPATCH(0x71, ADC, 5, IndirectIndexed);

    ADD_DISPATCH(0xE9, SBC, 2, Immediate);
    ADD_DISPATCH(0xE5, SBC, 3, ZeroPage);
    ADD_DISPATCH(0xF5, SBC, 4, ZeroPageX);
    ADD_DISPATCH(0xED, SBC, 4, Absolute);
    ADD_DISPATCH(0xFD, SBC, 4, AbsoluteX);
    ADD_DISPATCH(0xF9, SBC, 4, AbsoluteY);
    ADD_DISPATCH(0xE1, SBC, 6, IndexedIndirect);
    ADD_DISPATCH(0xF1, SBC, 5, IndirectIndexed);

    ADD_DISPATCH(0xC9, CMP, 2, Immediate);
    ADD_DISPATCH(0xC5, CMP, 3, ZeroPage);
    ADD_DISPATCH(0xD5, CMP, 4, ZeroPageX);
    ADD_DISPATCH(0xCD, CMP, 4, Absolute);
    ADD_DISPATCH(0xDD, CMP, 4, AbsoluteX);
    ADD_DISPATCH(0xD9, CMP, 4, AbsoluteY);
    ADD_DISPATCH(0xC1, CMP, 6, IndexedIndirect);
    ADD_DISPATCH(0xD1, CMP, 5, IndirectIndexed);

    ADD_DISPATCH(0xE0, CPX, 2, Immediate);
    ADD_DISPATCH(0xE4, CPX, 3, ZeroPage);
    ADD_DISPATCH(0xEC, CPX, 4, Absolute);

    ADD_DISPATCH(0xC0, CPY, 2, Immediate);
    ADD_DISPATCH(0xC4, CPY, 3, ZeroPage);
    ADD_DISPATCH(0xCC, CPY, 4, Absolute);

    // INCREMENT & DECREMENT OPERATIONS
    ADD_DISPATCH(0xE6, INC, 5, ZeroPage);
    ADD_DISPATCH(0xF6, INC, 6, ZeroPageX);
    ADD_DISPATCH(0xEE, INC, 6, Absolute);
    ADD_DISPATCH(0xFE, INC, 7, AbsoluteX);
    ADD_DISPATCH(0xE8, INX, 2, Implied);
    ADD_DISPATCH(0xC8, INY, 2, Implied);

    ADD_DISPATCH(0xC6, DEC, 5, ZeroPage);
    ADD_DISPATCH(0xD6, DEC, 6, ZeroPageX);
    ADD_DISPATCH(0xCE, DEC, 6, Absolute);
    ADD_DISPATCH(0xDE, DEC, 7, AbsoluteX);
    ADD_DISPATCH(0xCA, DEX, 2, Implied);
    ADD_DISPATCH(0x88, DEY, 2, Implied);

    // SHIFT OPERATIONS
    ADD_DISPATCH(0x0A, ASLA, 2, Accumulator);
    ADD_DISPATCH(0x06, ASL, 5, ZeroPage);
    ADD_DISPATCH(0x16, ASL, 6, ZeroPageX);
    ADD_DISPATCH(0x0E, ASL, 6, Absolute);
    ADD_DISPATCH(0x1E, ASL, 7, AbsoluteX);

    ADD_DISPATCH(0x4A, LSRA, 2, Accumulator);
    ADD_DISPATCH(0x46, LSR, 5, ZeroPage);
    ADD_DISPATCH(0x56, LSR, 6, ZeroPageX);
    ADD_DISPATCH(0x4E, LSR, 6, Absolute);
    ADD_DISPATCH(0x5E, LSR, 7, AbsoluteX);

    ADD_DISPATCH(0x2A, ROLA, 2, Accumulator);
    ADD_DISPATCH(0x26, ROL, 5, ZeroPage);
    ADD_DISPATCH(0x36, ROL, 6, ZeroPageX);
    ADD_DISPATCH(0x2E, ROL, 6, Absolute);
    ADD_DISPATCH(0x3E, ROL, 7, AbsoluteX);

    ADD_DISPATCH(0x6A, RORA, 2, Accumulator);
    ADD_DISPATCH(0x66, ROR, 5, ZeroPage);
    ADD_DISPATCH(0x76, ROR, 6, ZeroPageX);
    ADD_DISPATCH(0x6E, ROR, 6, Absolute);
    ADD_DISPATCH(0x7E, ROR, 7, AbsoluteX);

    // JUMPS & CALLS OPERATIONS
    ADD_DISPATCH(0x4C, JMP, 3, Absolute);
    ADD_DISPATCH(0x6C, JMP, 5, Indirect);
    ADD_DISPATCH(0x20, JSR, 6, Absolute);
    ADD_DISPATCH(0x60, RTS, 6, Implied);

    // BRANCH OPERATIONS
    ADD_DISPATCH(0x90, BCC, 2, Relative);
    ADD_DISPATCH(0xB0, BCS, 2, Relative);
    ADD_DISPATCH(0xF0, BEQ, 2, Relative);
    ADD_DISPATCH(0x30, BMI, 2, Relative);
    ADD_DISPATCH(0xD0, BNE, 2, Relative);
    ADD_DISPATCH(0x10, BPL, 2, Relative);
    ADD_DISPATCH(0x50, BVC, 2, Relative);
    ADD_DISPATCH(0x70, BVS, 2, Relative);
    // TAKE A LOOK AT THIS, IT IS 2 + 1 OR 2
    // STATUS FLAG OPERATIONS
    ADD_DISPATCH(0x18, CLC, 2, Implied);
    ADD_DISPATCH(0xD8, CLD, 2, Implied);
    ADD_DISPATCH(0x58, CLI, 2, Implied);
    ADD_DISPATCH(0xB8, CLV, 2, Implied);
    ADD_DISPATCH(0x38, SEC, 2, Implied);
    ADD_DISPATCH(0xF8, SED, 2, Implied);
    ADD_DISPATCH(0x78, SEI, 2, Implied);

    // // SYSTEM OPERATIONS
    ADD_DISPATCH(0x00, BRK, 7, Implied);
    ADD_DISPATCH(0xEA, NOP, 2, Implied);
    ADD_DISPATCH(0x40, RTI, 6, Implied);
}

void CPU::Reset(Mem& memory)
{
    PC = 0xFFFC;
    SP = 0xFF;

    // Clear processor status flags
    PS = 0b00000000;

    // Reset general-purpose registers
    A = 0;
    X = 0;
    Y = 0;

    // Fill memory with 0s.
    memory.Initialize();
}

// Fetch a single byte from memory offsetted by the PC.
uint8_t CPU::FetchByte(Mem& memory)
{
    uint8_t b = memory[PC];
    PC++;

    return b;
}

uint16_t CPU::FetchWord(Mem& memory)
{
    uint16_t w = memory[PC];
    w |= (memory[PC + 1] << 8);

    PC += 2;

    return w;
}

// Like FetchByte, except it fetches using an address and it does not
// increment the program counter.
uint8_t CPU::ReadByte(uint16_t address, Mem& memory)
{
    uint8_t b = memory[address];

    return b;
}

void CPU::StoreByte(uint16_t address, uint8_t value, Mem& memory)
{
    memory[address] = value;
}

uint16_t CPU::ReadWord(uint16_t address, Mem& memory)
{
    uint8_t l = ReadByte(address, memory);
    uint8_t h = ReadByte(address + 1, memory);

    return (h << 8) | l;
}

void CPU::StoreWord(uint16_t address, uint16_t value, Mem& memory)
{
    memory[address] = (uint8_t)value << 8;
    memory[address + 1] = (uint8_t)value >> 8;
}

void CPU::PushByteToStack(uint8_t value, Mem& memory)
{
    StoreByte(0x100 + SP--, value, memory);
}

void CPU::PushWordToStack(uint16_t value, Mem& memory)
{
    StoreByte(0x100 + SP--, value >> 8, memory);
    StoreByte(0x100 + SP--, value & 0xFF, memory);
}

uint8_t CPU::PullByteFromStack(Mem& memory)
{
    SP++;
    return ReadByte(0x100 + SP, memory);
}

uint16_t CPU::PullWordFromStack(Mem& memory)
{
    SP++;
    return ReadWord(0x100 + SP, memory);
}

// Instruction specific functions
void CPU::SetFlagsZN(uint8_t reg)
{
    Z = (reg == 0);
    N = (reg & 0b10000000) > 0;
}

void CPU::ConditionalBranch(bool flag, bool status, uint16_t address)
{
    int8_t relative_address = (int8_t)address;
    if (flag == status)
    {
        // If the branching is succesful, consume an extra cycle.
        consume_cycle = true;

        // If the branching crosses a page boundary, consume an extra cycle.
        page_crossed = ((PC >> 8) != ((PC + relative_address) >> 8));

        PC += relative_address;
    }
}

void CPU::ExecInstruction(Instruction instruction, uint32_t& machine_cycles, Mem& memory)
{
    uint16_t address = (this->*instruction.addr)(memory);
    (this->*instruction.op)(machine_cycles, address, memory);

    machine_cycles -= instruction.cycles;

    if (consume_cycle)
    {
        machine_cycles--;
        consume_cycle = false;
    }

    if (page_crossed)
    {
        machine_cycles--;
        page_crossed = false;
    }
}

uint32_t CPU::Execute(uint32_t machine_cycles, Mem& memory)
{
    const uint32_t machine_cycles_requested = machine_cycles;
    while (machine_cycles > 0)
    {
        uint8_t instruction = FetchByte(memory);
        Instruction ins = dispatch_table[instruction];
        ExecInstruction(ins, machine_cycles, memory);
    }

    const uint32_t machine_cycles_used = machine_cycles_requested - machine_cycles;
    return machine_cycles_used;
}

// Addressing mode functions
uint16_t CPU::AddrOpcode(Mem& memory)
{
    return memory[PC - 1];
}

uint16_t CPU::AddrAccumulator(Mem&)
{
    return A;
}

uint16_t CPU::AddrImplied(Mem&)
{
    return 0;
}

uint16_t CPU::AddrImmediate(Mem&)
{
    return PC++;
}

uint16_t CPU::AddrZeroPage(Mem& memory)
{
    return FetchByte(memory);
}

uint16_t CPU::AddrZeroPageX(Mem& memory)
{
    uint8_t zeropage_address = FetchByte(memory);

    // If it exceeds the zero page, wrap around.
    uint8_t zeropage_addressX = (zeropage_address + X) & 0xFF;

    return zeropage_addressX;
}

uint16_t CPU::AddrZeroPageY(Mem& memory)
{
    uint8_t zeropage_address = FetchByte(memory);

    // If it exceeds the zero page, wrap around.
    uint8_t zeropage_addressY = (zeropage_address + Y) & 0xFF;

    return zeropage_addressY;
}

uint16_t CPU::AddrAbsolute(Mem& memory)
{
    return FetchWord(memory);
}

uint16_t CPU::AddrAbsoluteX(Mem& memory)
{
    uint16_t abs_address = FetchWord(memory);
    uint16_t sum = abs_address + X;

    // If the zero page is crossed
    page_crossed = ((abs_address ^ sum) >> 8);

    return sum;
}

uint16_t CPU::AddrAbsoluteX5(Mem& memory)
{
    uint16_t abs_address = FetchWord(memory);
    uint16_t sum = abs_address + X;

    return sum;
}

uint16_t CPU::AddrAbsoluteY(Mem& memory)
{
    uint16_t abs_address = FetchWord(memory);
    uint16_t sum = abs_address + Y;

    // If the zero page is crossed
    page_crossed = ((abs_address ^ sum) >> 8);

    return sum;
}

uint16_t CPU::AddrAbsoluteY5(Mem& memory)
{
    uint16_t abs_address = FetchWord(memory);
    uint16_t sum = abs_address + Y;

    return sum;
}

// The indirect addressing mode is designed to reproduce a original
// bug of the 6502 where a jumping to a vector starting at the last
// byte of the page will use the high byte of the last byte in the
// page and the low byte of the first byte in the page.
uint16_t CPU::AddrIndirect(Mem& memory)
{
    uint8_t l = FetchByte(memory);
    uint8_t h = FetchByte(memory);

    uint8_t a = ReadByte((uint16_t)(h << 8) | l, memory);
    uint8_t b = ReadByte((uint16_t)(h << 8) | ((l + 1) & 0xFF), memory);

    return (uint16_t)(b << 8) | a;
}

uint16_t CPU::AddrIndexedIndirect(Mem& memory)
{
    uint16_t address = FetchByte(memory);
    uint16_t target = ReadWord((address + X) & 0xFF, memory);

    return target;
}

uint16_t CPU::AddrIndirectIndexed(Mem& memory)
{
    uint8_t zeropage_address = FetchByte(memory);
    uint16_t target = ReadWord(zeropage_address, memory);
    uint16_t targetY = target + Y;

    // If the zero page is crossed
    page_crossed = ((target ^ targetY) >> 8);

    return targetY;
}

uint16_t CPU::AddrIndirectIndexed6(Mem& memory)
{
    uint8_t zeropage_address = FetchByte(memory);
    uint16_t target = ReadWord(zeropage_address, memory);
    uint16_t targetY = target + Y;

    return targetY;
}

uint16_t CPU::AddrRelative(Mem& memory)
{
    return FetchByte(memory);
}

// Instruction functions
void CPU::OpLDA(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    A = ReadByte(address, memory);
    SetFlagsZN(A);
}

void CPU::OpLDX(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    X = ReadByte(address, memory);
    SetFlagsZN(X);
}

void CPU::OpLDY(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    Y = ReadByte(address, memory);
    SetFlagsZN(Y);
}

void CPU::OpSTA(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    StoreByte(address, A, memory);
}

void CPU::OpSTX(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    StoreByte(address, X, memory);
}

void CPU::OpSTY(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    StoreByte(address, Y, memory);
}

void CPU::OpTAX(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    X = A;
    SetFlagsZN(X);
}
void CPU::OpTAY(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    Y = A;
    SetFlagsZN(Y);
}
void CPU::OpTXA(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    A = X;
    SetFlagsZN(A);
}
void CPU::OpTYA(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    A = Y;
    SetFlagsZN(A);
}

void CPU::OpTSX(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    X = SP;
    SetFlagsZN(X);
}

void CPU::OpTXS(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    SP = X;
}

void CPU::OpPHA(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    PushByteToStack(A, memory);
}

void CPU::OpPHP(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    PushByteToStack(PS, memory);
}

void CPU::OpPLA(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    A = PullByteFromStack(memory);
    SetFlagsZN(A);
}

void CPU::OpPLP(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    PS = PullByteFromStack(memory);
}

void CPU::OpAND(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    A &= ReadByte(address, memory);
    SetFlagsZN(A);
}

void CPU::OpEOR(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    A ^= ReadByte(address, memory);
    SetFlagsZN(A);
}

void CPU::OpORA(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    A |= ReadByte(address, memory);
    SetFlagsZN(A);
}

void CPU::OpBIT(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    uint8_t result = ReadByte(address, memory) & A;

    Z = (result == 0);
    V = result & 0x40;
    N = (result & 0b1000000) > 0;
}

void CPU::OpADC(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    uint8_t operand = ReadByte(address, memory);
    const bool sign_bits_match = !(operand & 0b10000000) ^ (A & 0b10000000);

    uint16_t sum = A + C + operand;
    A = (sum & 0xFF);

    // The addition overflowed if the sign bit of the
    // operand and the pre-addition accumulator matched and...
    // if the sign bit of the pre-op accumulator and
    // the result differ.
    V = sign_bits_match && ((A ^ operand) & 0b10000000);
    Z = (A == 0);
    N = (A & 0b10000000) > 0;
    C = (sum > 0xFF);
}

void CPU::OpSBC(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    // Subtraction is the same as addition with the negated operand.
    memory[address] = ~memory[address];
    OpADC(machine_cycles, address, memory);
}

void CPU::OpCMP(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    uint8_t operand = ReadByte(address, memory);
    C = (A >= operand);
    Z = (A == operand);
    N = ((A - operand) & 0b10000000) > 0;
}

void CPU::OpCPX(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    uint8_t operand = ReadByte(address, memory);
    C = (X >= operand);
    Z = (X == operand);
    N = ((X - operand) & 0b10000000) > 0;
}

void CPU::OpCPY(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    uint8_t operand = ReadByte(address, memory);
    C = (Y >= operand);
    Z = (Y == operand);
    N = ((Y - operand) & 0b10000000) > 0;
}

void CPU::OpINC(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    uint8_t result = ReadByte(address, memory);
    result++;
    StoreByte(address, result, memory);
    SetFlagsZN(result);
}

void CPU::OpINX(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    X++;
    SetFlagsZN(X);
}

void CPU::OpINY(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    Y++;

    SetFlagsZN(Y);
}

void CPU::OpDEC(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    uint8_t result = ReadByte(address, memory);
    result--;
    StoreByte(address, result, memory);
    SetFlagsZN(result);
}

void CPU::OpDEX(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    X--;
    SetFlagsZN(X);
}

void CPU::OpDEY(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    Y--;
    SetFlagsZN(Y);
}

void CPU::OpASLA(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    C = (A & 0b10000000) > 0;
    A <<= 1;
    SetFlagsZN(A);
}

void CPU::OpASL(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    uint8_t operand = ReadByte(address, memory);
    C = (operand & 0b10000000) > 0;

    operand <<= 1;
    StoreByte(address, operand, memory);
    SetFlagsZN(operand);
}

void CPU::OpLSRA(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    C = (A & 0b00000001);
    A >>= 1;
    SetFlagsZN(A);
}

void CPU::OpLSR(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    uint8_t operand = ReadByte(address, memory);
    C = (operand & 0b00000001);

    operand >>= 1;
    StoreByte(address, operand, memory);
    SetFlagsZN(operand);
}

void CPU::OpROLA(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    const uint8_t operand = A;
    A <<= 1;

    if (C)
        A |= 0b00000001;

    C = (operand & 0b10000000) > 0;
    SetFlagsZN(A);
}

void CPU::OpROL(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    const uint8_t operand = ReadByte(address, memory);
    uint8_t result = operand << 1;

    if (C)
        result |= 0b00000001;

    StoreByte(address, result, memory);
    C = (operand & 0b10000000) > 0;
    SetFlagsZN(result);
}

void CPU::OpRORA(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    const uint8_t operand = A;
    A >>= 1;

    if (C)
        A |= 0b10000000;

    C = (operand & 0b00000001) > 0;
    SetFlagsZN(A);
}

void CPU::OpROR(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    const uint8_t operand = ReadByte(address, memory);
    uint8_t result = operand >> 1;

    if (C)
        result |= 0b10000000;

    StoreByte(address, result, memory);
    C = (operand & 0b00000001) > 0;
    SetFlagsZN(result);
}

// Implements the bug the 6502 has with jumping in the indirect addressing function.
void CPU::OpJMP(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    PC = address;
}

void CPU::OpJSR(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    PushWordToStack(PC - 1, memory);
    PC = address;
}

void CPU::OpRTS(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    PC = PullWordFromStack(memory);
}

void CPU::OpBCC(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    ConditionalBranch(C, false, address);
}

void CPU::OpBCS(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    ConditionalBranch(C, true, address);
}

void CPU::OpBEQ(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    ConditionalBranch(Z, true, address);
}

void CPU::OpBMI(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    ConditionalBranch(N, true, address);
}

void CPU::OpBNE(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    ConditionalBranch(Z, false, address);
}

void CPU::OpBPL(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    ConditionalBranch(N, false, address);
}

void CPU::OpBVC(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    ConditionalBranch(V, false, address);
}

void CPU::OpBVS(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    ConditionalBranch(V, true, address);
}

void CPU::OpCLC(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    C = false;
}

void CPU::OpCLD(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    D = false;
}

void CPU::OpCLI(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    I = false;
}

void CPU::OpCLV(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    V = false;
}

void CPU::OpSEC(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    C = true;
}

void CPU::OpSED(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    D = true;
}

void CPU::OpSEI(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    I = true;
}

void CPU::OpBRK(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    PushWordToStack(PC, memory);
    PushByteToStack(PS, memory);
    PC = FetchWord(memory);
    B = true;
}

void CPU::OpNOP(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    ;
}

void CPU::OpRTI(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    PS = PullByteFromStack(memory);
    PC = PullWordFromStack(memory);
}

void CPU::OpIllegal(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    std::stringstream stream;
    stream << "Unhandled instruction: 0x" << std::hex << address;
    throw std::invalid_argument(stream.str());
}