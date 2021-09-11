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

#define ADD_DISPATCH(HEX, NAME, ADDRESSING_MODE)    \
    instruction.addr = &CPU::Addr##ADDRESSING_MODE; \
    instruction.op = &CPU::Op##NAME;                \
    dispatch_table[HEX] = instruction

CPU::CPU()
{
    // Prefill dispatch table with illegal opcode handlers
    Instruction instruction;
    instruction.addr = &CPU::AddrOpcode;
    instruction.op = &CPU::OpIllegal;
    dispatch_table.fill(instruction);

    // LOAD & STORE
    ADD_DISPATCH(0xA9, LDA, Immediate);
    ADD_DISPATCH(0xA5, LDA, ZeroPage);
    ADD_DISPATCH(0xB5, LDA, ZeroPageX);
    ADD_DISPATCH(0x6D, LDA, Absolute);
    ADD_DISPATCH(0xBD, LDA, AbsoluteX);
    ADD_DISPATCH(0xB9, LDA, AbsoluteY);
    ADD_DISPATCH(0xA1, LDA, IndexedIndirect);
    ADD_DISPATCH(0xB1, LDA, IndirectIndexed);

    ADD_DISPATCH(0xA2, LDX, Immediate);
    ADD_DISPATCH(0xA6, LDX, ZeroPage);
    ADD_DISPATCH(0xB6, LDX, ZeroPageY);
    ADD_DISPATCH(0xAE, LDX, Absolute);
    ADD_DISPATCH(0xBE, LDX, AbsoluteY);

    ADD_DISPATCH(0xA0, LDY, Immediate);
    ADD_DISPATCH(0xA4, LDY, ZeroPage);
    ADD_DISPATCH(0xB4, LDY, ZeroPageX);
    ADD_DISPATCH(0xAC, LDY, Absolute);
    ADD_DISPATCH(0xBC, LDY, AbsoluteX);

    ADD_DISPATCH(0x85, STA, ZeroPage);
    ADD_DISPATCH(0x95, STA, ZeroPageX);
    ADD_DISPATCH(0x8D, STA, Absolute);
    ADD_DISPATCH(0x9D, STA, AbsoluteX5);
    ADD_DISPATCH(0x99, STA, AbsoluteY5);
    ADD_DISPATCH(0x81, STA, IndexedIndirect);
    ADD_DISPATCH(0x91, STA, IndirectIndexed6);

    ADD_DISPATCH(0x86, STX, ZeroPage);
    ADD_DISPATCH(0x96, STX, ZeroPageY);
    ADD_DISPATCH(0x8e, STX, Absolute);

    ADD_DISPATCH(0x84, STY, ZeroPage);
    ADD_DISPATCH(0x94, STY, ZeroPageX);
    ADD_DISPATCH(0x8C, STY, Absolute);

    // REGISTER TRANSFERS
    ADD_DISPATCH(0xAA, TAX, Implied);
    ADD_DISPATCH(0xA8, TAY, Implied);
    ADD_DISPATCH(0x8A, TXA, Implied);
    ADD_DISPATCH(0x98, TYA, Implied);

    // STACK OPERATIONS
    ADD_DISPATCH(0xBA, TSX, Implied);
    ADD_DISPATCH(0x9A, TXS, Implied);
    ADD_DISPATCH(0x48, PHA, Implied);
    ADD_DISPATCH(0x08, PHP, Implied);
    ADD_DISPATCH(0x68, PLA, Implied);
    ADD_DISPATCH(0x28, PLP, Implied);

    // LOGICAL OPERATIONS
    ADD_DISPATCH(0x29, AND, Immediate);
    ADD_DISPATCH(0x25, AND, ZeroPage);
    ADD_DISPATCH(0x35, AND, ZeroPageX);
    ADD_DISPATCH(0x2D, AND, Absolute);
    ADD_DISPATCH(0x3D, AND, AbsoluteX);
    ADD_DISPATCH(0x39, AND, AbsoluteY);
    ADD_DISPATCH(0x21, AND, IndexedIndirect);
    ADD_DISPATCH(0x31, AND, IndirectIndexed);

    ADD_DISPATCH(0x49, EOR, Immediate);
    ADD_DISPATCH(0x45, EOR, ZeroPage);
    ADD_DISPATCH(0x55, EOR, ZeroPageX);
    ADD_DISPATCH(0x4D, EOR, Absolute);
    ADD_DISPATCH(0x5D, EOR, AbsoluteX);
    ADD_DISPATCH(0x59, EOR, AbsoluteY);
    ADD_DISPATCH(0x41, EOR, IndexedIndirect);
    ADD_DISPATCH(0x51, EOR, IndirectIndexed);

    ADD_DISPATCH(0x09, ORA, Immediate);
    ADD_DISPATCH(0x05, ORA, ZeroPage);
    ADD_DISPATCH(0x15, ORA, ZeroPageX);
    ADD_DISPATCH(0x0D, ORA, Absolute);
    ADD_DISPATCH(0x1D, ORA, AbsoluteX);
    ADD_DISPATCH(0x19, ORA, AbsoluteY);
    ADD_DISPATCH(0x01, ORA, IndexedIndirect);
    ADD_DISPATCH(0x11, ORA, IndirectIndexed);

    ADD_DISPATCH(0x24, BIT, ZeroPage);
    ADD_DISPATCH(0x2C, BIT, Absolute);

    // ARITHMETIC OPERATIONS
    ADD_DISPATCH(0x69, ADC, Immediate);
    ADD_DISPATCH(0x65, ADC, ZeroPage);
    ADD_DISPATCH(0x75, ADC, ZeroPageX);
    ADD_DISPATCH(0x6D, ADC, Absolute);
    ADD_DISPATCH(0x7D, ADC, AbsoluteX);
    ADD_DISPATCH(0x79, ADC, AbsoluteY);
    ADD_DISPATCH(0x61, ADC, IndexedIndirect);
    ADD_DISPATCH(0x71, ADC, IndirectIndexed);

    ADD_DISPATCH(0xE9, SBC, Immediate);
    ADD_DISPATCH(0xE5, SBC, ZeroPage);
    ADD_DISPATCH(0xF5, SBC, ZeroPageX);
    ADD_DISPATCH(0xED, SBC, Absolute);
    ADD_DISPATCH(0xFD, SBC, AbsoluteX);
    ADD_DISPATCH(0xF9, SBC, AbsoluteY);
    ADD_DISPATCH(0xE1, SBC, IndexedIndirect);
    ADD_DISPATCH(0xF1, SBC, IndirectIndexed);

    ADD_DISPATCH(0xC9, CMP, Immediate);
    ADD_DISPATCH(0xC5, CMP, ZeroPage);
    ADD_DISPATCH(0xD5, CMP, ZeroPageX);
    ADD_DISPATCH(0xCD, CMP, Absolute);
    ADD_DISPATCH(0xDD, CMP, AbsoluteX);
    ADD_DISPATCH(0xD9, CMP, AbsoluteY);
    ADD_DISPATCH(0xC1, CMP, IndexedIndirect);
    ADD_DISPATCH(0xD1, CMP, IndirectIndexed);

    ADD_DISPATCH(0xE0, CPX, Immediate);
    ADD_DISPATCH(0xE4, CPX, ZeroPage);
    ADD_DISPATCH(0xEC, CPX, Absolute);

    ADD_DISPATCH(0xC0, CPY, Immediate);
    ADD_DISPATCH(0xC4, CPY, ZeroPage);
    ADD_DISPATCH(0xCC, CPY, Absolute);

    // INCREMENTS & DECREMENTS OPERATIONS
    ADD_DISPATCH(0xE6, INC, ZeroPage);
    ADD_DISPATCH(0xF6, INC, ZeroPageX);
    ADD_DISPATCH(0xEE, INC, Absolute);
    ADD_DISPATCH(0xFE, INC, AbsoluteX);
    ADD_DISPATCH(0xE8, INX, Implied);
    ADD_DISPATCH(0xC8, INY, Implied);

    ADD_DISPATCH(0xC6, DEC, ZeroPage);
    ADD_DISPATCH(0xD6, DEC, ZeroPageX);
    ADD_DISPATCH(0xCE, DEC, Absolute);
    ADD_DISPATCH(0xDE, DEC, AbsoluteX);
    ADD_DISPATCH(0xCA, DEX, Implied);
    ADD_DISPATCH(0x88, DEY, Implied);

    // SHIFTS OPERATIONS
    ADD_DISPATCH(0x0A, ASL_A, Accumulator);
    ADD_DISPATCH(0x06, ASL, ZeroPage);
    ADD_DISPATCH(0x16, ASL, ZeroPageX);
    ADD_DISPATCH(0x0E, ASL, Absolute);
    ADD_DISPATCH(0x1E, ASL, AbsoluteX);
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
uint8_t CPU::FetchByte(uint32_t& machine_cycles, Mem& memory)
{
    uint8_t b = memory[PC];
    PC++;
    machine_cycles--;

    return b;
}

uint16_t CPU::FetchWord(uint32_t& machine_cycles, Mem& memory)
{
    uint16_t w = memory[PC];
    w |= (memory[PC + 1] << 8);

    PC += 2;
    machine_cycles -= 2;

    return w;
}

// Like FetchByte, except it fetches using an address and it does not
// increment the program counter.
uint8_t CPU::ReadByte(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    uint8_t b = memory[address];
    machine_cycles--;

    return b;
}

void CPU::StoreByte(uint32_t& machine_cycles, uint16_t address, uint8_t value, Mem& memory)
{
    memory[address] = value;
    machine_cycles--;
}

uint16_t CPU::ReadWord(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    uint8_t l = ReadByte(machine_cycles, address, memory);
    uint8_t h = ReadByte(machine_cycles, address + 1, memory);

    return (h << 8) | l;
}

void CPU::PushToStack(uint32_t& machine_cycles, uint8_t value, Mem& memory)
{
    StoreByte(machine_cycles, 0x100 + SP--, value, memory);
    machine_cycles--;
}

uint8_t CPU::PullFromStack(uint32_t& machine_cycles, Mem& memory)
{
    SP++;
    machine_cycles -= 2;  // Why does this consume three cycles (four total)?

    return ReadByte(machine_cycles, 0x100 + SP, memory);
}

// Instruction specific functions
void CPU::SetFlagsZN(uint8_t reg)
{
    Z = (reg == 0);
    N = (reg & 0b10000000) > 0;
}

void CPU::ExecInstruction(Instruction instruction, uint32_t& machine_cycles, Mem& memory)
{
    uint16_t address = (this->*instruction.addr)(machine_cycles, memory);
    (this->*instruction.op)(machine_cycles, address, memory);
}

uint32_t CPU::Execute(uint32_t machine_cycles, Mem& memory)
{
    const uint32_t machine_cycles_requested = machine_cycles;
    while (machine_cycles > 0)
    {
        uint8_t instruction = FetchByte(machine_cycles, memory);
        Instruction ins = dispatch_table[instruction];
        ExecInstruction(ins, machine_cycles, memory);
    }

    const uint32_t machine_cycles_used = machine_cycles_requested - machine_cycles;
    return machine_cycles_used;
}

// Addressing mode functions
uint16_t CPU::AddrOpcode(uint32_t&, Mem& memory)
{
    return memory[PC - 1];
}

uint16_t CPU::AddrAccumulator(uint32_t&, Mem&)
{
    return A;
}

uint16_t CPU::AddrImplied(uint32_t&, Mem&)
{
    return 0;
}

uint16_t CPU::AddrImmediate(uint32_t&, Mem&)
{
    return PC++;
}

uint16_t CPU::AddrZeroPage(uint32_t& machine_cycles, Mem& memory)
{
    return FetchByte(machine_cycles, memory);
}

uint16_t CPU::AddrZeroPageX(uint32_t& machine_cycles, Mem& memory)
{
    uint8_t zeropage_address = FetchByte(machine_cycles, memory);

    // If it exceeds the zero page, wrap around.
    uint8_t zeropage_addressX = (zeropage_address + X) & 0xFF;
    machine_cycles--;

    return zeropage_addressX;
}

uint16_t CPU::AddrZeroPageY(uint32_t& machine_cycles, Mem& memory)
{
    uint8_t zeropage_address = FetchByte(machine_cycles, memory);

    // If it exceeds the zero page, wrap around.
    uint8_t zeropage_addressY = (zeropage_address + Y) & 0xFF;
    machine_cycles--;

    return zeropage_addressY;
}

uint16_t CPU::AddrAbsolute(uint32_t& machine_cycles, Mem& memory)
{
    return FetchWord(machine_cycles, memory);
}

uint16_t CPU::AddrAbsoluteX(uint32_t& machine_cycles, Mem& memory)
{
    uint16_t abs_address = FetchWord(machine_cycles, memory);
    uint16_t sum = abs_address + X;

    // If the zero page is crossed
    if ((abs_address ^ sum) >> 8)
        machine_cycles--;

    return sum;
}

uint16_t CPU::AddrAbsoluteX5(uint32_t& machine_cycles, Mem& memory)
{
    uint16_t abs_address = FetchWord(machine_cycles, memory);
    uint16_t sum = abs_address + X;
    machine_cycles--;

    return sum;
}

uint16_t CPU::AddrAbsoluteY(uint32_t& machine_cycles, Mem& memory)
{
    uint16_t abs_address = FetchWord(machine_cycles, memory);
    uint16_t sum = abs_address + Y;

    // If the zero page is crossed
    if ((abs_address ^ sum) >> 8)
        machine_cycles--;

    return sum;
}

uint16_t CPU::AddrAbsoluteY5(uint32_t& machine_cycles, Mem& memory)
{
    uint16_t abs_address = FetchWord(machine_cycles, memory);
    uint16_t sum = abs_address + Y;
    machine_cycles--;

    return sum;
}

uint16_t CPU::AddrIndexedIndirect(uint32_t& machine_cycles, Mem& memory)
{
    uint16_t address = FetchByte(machine_cycles, memory);
    uint16_t target = ReadWord(machine_cycles, (address + X) & 0xFF, memory);
    machine_cycles--;

    return target;
}

uint16_t CPU::AddrIndirectIndexed(uint32_t& machine_cycles, Mem& memory)
{
    uint8_t zeropage_address = FetchByte(machine_cycles, memory);
    uint16_t target = ReadWord(machine_cycles, zeropage_address, memory);
    uint16_t targetY = target + Y;

    // If the zero page is crossed
    if ((target ^ targetY) >> 8)
        machine_cycles--;

    return targetY;
}

uint16_t CPU::AddrIndirectIndexed6(uint32_t& machine_cycles, Mem& memory)
{
    uint8_t zeropage_address = FetchByte(machine_cycles, memory);
    uint16_t target = ReadWord(machine_cycles, zeropage_address, memory);
    uint16_t targetY = target + Y;
    machine_cycles--;

    return targetY;
}

// Instruction functions
void CPU::OpLDA(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    A = ReadByte(machine_cycles, address, memory);
    SetFlagsZN(A);
}

void CPU::OpLDX(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    X = ReadByte(machine_cycles, address, memory);
    SetFlagsZN(X);
}

void CPU::OpLDY(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    Y = ReadByte(machine_cycles, address, memory);
    SetFlagsZN(Y);
}

void CPU::OpSTA(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    StoreByte(machine_cycles, address, A, memory);
}

void CPU::OpSTX(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    StoreByte(machine_cycles, address, X, memory);
}

void CPU::OpSTY(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    StoreByte(machine_cycles, address, Y, memory);
}

void CPU::OpTAX(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    X = A;
    machine_cycles--;
    SetFlagsZN(X);
}
void CPU::OpTAY(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    Y = A;
    machine_cycles--;
    SetFlagsZN(Y);
}
void CPU::OpTXA(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    A = X;
    machine_cycles--;
    SetFlagsZN(A);
}
void CPU::OpTYA(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    A = Y;
    machine_cycles--;
    SetFlagsZN(A);
}

void CPU::OpTSX(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    X = SP;
    machine_cycles--;
    SetFlagsZN(X);
}

void CPU::OpTXS(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    SP = X;
    machine_cycles--;
}

void CPU::OpPHA(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    PushToStack(machine_cycles, A, memory);
}

void CPU::OpPHP(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    PushToStack(machine_cycles, PS, memory);
}

void CPU::OpPLA(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    A = PullFromStack(machine_cycles, memory);
    SetFlagsZN(A);
}

void CPU::OpPLP(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    PS = PullFromStack(machine_cycles, memory);
}

void CPU::OpAND(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    A &= ReadByte(machine_cycles, address, memory);
    SetFlagsZN(A);
}

void CPU::OpEOR(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    A ^= ReadByte(machine_cycles, address, memory);
    SetFlagsZN(A);
}

void CPU::OpORA(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    A |= ReadByte(machine_cycles, address, memory);
    SetFlagsZN(A);
}

void CPU::OpBIT(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    uint8_t result = ReadByte(machine_cycles, address, memory) & A;

    Z = (result == 0);
    V = result & 0x40;
    N = (result & 0b1000000) > 0;
}

void CPU::OpADC(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    uint8_t operand = ReadByte(machine_cycles, address, memory);
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
    uint8_t operand = ReadByte(machine_cycles, address, memory);
    C = (A >= operand);
    Z = (A == operand);
    N = ((A - operand) & 0b10000000) > 0;
}

void CPU::OpCPX(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    uint8_t operand = ReadByte(machine_cycles, address, memory);
    C = (X >= operand);
    Z = (X == operand);
    N = ((X - operand) & 0b10000000) > 0;
}

void CPU::OpCPY(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    uint8_t operand = ReadByte(machine_cycles, address, memory);
    C = (Y >= operand);
    Z = (Y == operand);
    N = ((Y - operand) & 0b10000000) > 0;
}

void CPU::OpINC(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    uint8_t result = ReadByte(machine_cycles, address, memory);
    result++;
    machine_cycles--;
    StoreByte(machine_cycles, address, result, memory);
    SetFlagsZN(result);
}

void CPU::OpINX(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    X++;
    machine_cycles--;
    SetFlagsZN(X);
}

void CPU::OpINY(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    Y++;
    machine_cycles--;
    SetFlagsZN(Y);
}

void CPU::OpDEC(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    uint8_t result = ReadByte(machine_cycles, address, memory);
    result--;
    machine_cycles--;
    StoreByte(machine_cycles, address, result, memory);
    SetFlagsZN(result);
}

void CPU::OpDEX(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    X--;
    machine_cycles--;
    SetFlagsZN(X);
}

void CPU::OpDEY(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    Y--;
    machine_cycles--;
    SetFlagsZN(Y);
}

void CPU::OpASL_A(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    C = (A & 0b10000000) > 0;
    A <<= 1;
    machine_cycles--;
    SetFlagsZN(A);
}

void CPU::OpASL(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    uint8_t operand = ReadByte(machine_cycles, address, memory);
    C = (operand & 0b10000000) > 0;

    operand <<= 1;
    machine_cycles--;
    StoreByte(machine_cycles, address, operand, memory);
    SetFlagsZN(operand);
}

void CPU::OpIllegal(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    std::stringstream stream;
    stream << "Unhandled instruction: 0x" << std::hex << address;
    throw std::invalid_argument(stream.str());
}