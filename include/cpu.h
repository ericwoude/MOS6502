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

#ifndef CPU_H
#define CPU_H

#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>

#include "mem.h"

class CPU
{
   public:
    CPU();
    void Reset(Mem& memory);
    uint32_t Execute(uint32_t machine_cycles, Mem& memory);

    // Program counter, stack pointer and general-purpose registers A, X and Y.
    uint16_t PC;
    uint8_t SP;
    uint8_t A, X, Y;

    // Processor status flags
    union
    {
        uint8_t PS;
        struct
        {
            uint8_t C : 1;
            uint8_t Z : 1;
            uint8_t I : 1;
            uint8_t D : 1;
            uint8_t B : 1;
            uint8_t _ : 1;  // Unused last bit
            uint8_t V : 1;
            uint8_t N : 1;
        };
    };

   private:
    using AddressExecution = uint16_t (CPU::*)(uint32_t&, Mem&);
    using OperationExecution = void (CPU::*)(uint32_t&, uint16_t, Mem&);

    struct Instruction
    {
        AddressExecution addr;
        OperationExecution op;
    };

    std::array<Instruction, 256> dispatch_table;
    void ExecInstruction(Instruction instruction, uint32_t& machine_cycles, Mem& memory);

    // Addressing mode functions
    uint16_t AddrOpcode(uint32_t&, Mem& memory);  // Used for debugging illegal opcodes
    uint16_t AddrAccumulator(uint32_t&, Mem&);
    uint16_t AddrImplied(uint32_t&, Mem&);        // Does not do anything
    uint16_t AddrImmediate(uint32_t&, Mem&);
    uint16_t AddrZeroPage(uint32_t& machine_cycles, Mem& memory);
    uint16_t AddrZeroPageX(uint32_t& machine_cycles, Mem& memory);
    uint16_t AddrZeroPageY(uint32_t& machine_cycles, Mem& memory);
    uint16_t AddrAbsolute(uint32_t& machine_cycles, Mem& memory);
    uint16_t AddrAbsoluteX(uint32_t& machine_cycles, Mem& memory);
    uint16_t AddrAbsoluteX5(uint32_t& machine_cycles, Mem& memory);
    uint16_t AddrAbsoluteY(uint32_t& machine_cycles, Mem& memory);
    uint16_t AddrAbsoluteY5(uint32_t& machine_cycles, Mem& memory);
    uint16_t AddrIndexedIndirect(uint32_t& machine_cycles, Mem& memory);
    uint16_t AddrIndirectIndexed(uint32_t& machine_cycles, Mem& memory);
    uint16_t AddrIndirectIndexed6(uint32_t& machine_cycles, Mem& memory);

    // Sets the Z, N flag for the LDA, LDX and LDY instructions.
    void SetFlagsZN(uint8_t reg);

    // Fetch a single byte from memory offsetted by the PC.
    uint8_t FetchByte(uint32_t& machine_cycles, Mem& memory);
    uint16_t FetchWord(uint32_t& machine_cycles, Mem& memory);

    // Like FetchByte, except it fetches using an address and it does not
    // increment the program counter.
    uint8_t ReadByte(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void StoreByte(uint32_t& machine_cycles, uint16_t address, uint8_t value, Mem& memory);

    uint16_t ReadWord(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    // void StoreWord(uint32_t& machine_cycles, uint16_t address, uint16_t value, Mem& memory);

    // Stack operations
    void PushToStack(uint32_t& machine_cycles, uint8_t value, Mem& memory);
    uint8_t PullFromStack(uint32_t& machine_cycles, Mem& memory);

    // Operation functions
    // LOAD & STORE
    void OpLDA(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpLDX(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpLDY(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpSTA(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpSTX(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpSTY(uint32_t& machine_cycles, uint16_t address, Mem& memory);

    // REGISTER TRANSFERS
    void OpTAX(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpTAY(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpTXA(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpTYA(uint32_t& machine_cycles, uint16_t address, Mem& memory);

    // STACK OPERATIONS
    void OpTSX(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpTXS(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpPHA(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpPHP(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpPLA(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpPLP(uint32_t& machine_cycles, uint16_t address, Mem& memory);

    // LOGICAL OPERATIONS
    void OpAND(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpEOR(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpORA(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpBIT(uint32_t& machine_cycles, uint16_t address, Mem& memory);

    // ARITHMETIC OPERATIONS
    void OpADC(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpSBC(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpCMP(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpCPX(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpCPY(uint32_t& machine_cycles, uint16_t address, Mem& memory);

    // INCREMENTS & DECREMENTS OPERATIONS
    void OpINC(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpINX(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpINY(uint32_t& machine_cycles, uint16_t address, Mem& memory);

    void OpDEC(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpDEX(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpDEY(uint32_t& machine_cycles, uint16_t address, Mem& memory);

    // SHIFTS OPERATIONS
    void OpASL_A(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpASL(uint32_t& machine_cycles, uint16_t address, Mem& memory);

    void OpIllegal(uint32_t& machine_cycles, uint16_t address, Mem& memory);
};

#endif  // CPU_H