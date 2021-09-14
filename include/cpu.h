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
    using AddressExecution = uint16_t (CPU::*)(Mem&);
    using OperationExecution = void (CPU::*)(uint16_t, Mem&);

    struct Instruction
    {
        AddressExecution addr;
        OperationExecution op;
        uint8_t cycles;
    };

    std::array<Instruction, 256> dispatch_table;
    void ExecInstruction(Instruction instruction, uint32_t& machine_cycles, Mem& memory);

    // Addressing mode functions
    uint16_t AddrOpcode(Mem& memory);  // Used for debugging illegal opcodes
    uint16_t AddrAccumulator(Mem& memory);
    uint16_t AddrImplied(Mem& memory);  // Does not do anything
    uint16_t AddrImmediate(Mem& memory);
    uint16_t AddrZeroPage(Mem& memory);
    uint16_t AddrZeroPageX(Mem& memory);
    uint16_t AddrZeroPageY(Mem& memory);
    uint16_t AddrAbsolute(Mem& memory);
    uint16_t AddrAbsoluteX(Mem& memory);
    uint16_t AddrAbsoluteX5(Mem& memory);
    uint16_t AddrAbsoluteY(Mem& memory);
    uint16_t AddrAbsoluteY5(Mem& memory);
    uint16_t AddrIndirect(Mem& memory);
    uint16_t AddrIndexedIndirect(Mem& memory);
    uint16_t AddrIndirectIndexed(Mem& memory);
    uint16_t AddrIndirectIndexed6(Mem& memory);
    uint16_t AddrRelative(Mem& memory);

    // If branching operations are succesful, they consume a cycle, so does NOP.
    // Some instructions consume an extra cycle if the zero page is crossed.
    bool consume_cycle = false;
    bool page_crossed = false;

    // Sets the Z, N flag for the LDA, LDX and LDY instructions
    void SetFlagsZN(uint8_t reg);

    // Used for all branching operations
    void ConditionalBranch(bool flag, bool status, uint16_t address);

    // Fetch a single byte from memory offsetted by the PC
    uint8_t FetchByte(Mem& memory);
    uint16_t FetchWord(Mem& memory);

    // Like FetchByte, except it fetches using an address and it does not
    // increment the program counter
    uint8_t ReadByte(uint16_t address, Mem& memory);
    void StoreByte(uint16_t address, uint8_t value, Mem& memory);

    uint16_t ReadWord(uint16_t address, Mem& memory);
    void StoreWord(uint16_t address, uint16_t value, Mem& memory);

    // Stack operations
    void PushByteToStack(uint8_t value, Mem& memory);
    void PushWordToStack(uint16_t value, Mem& memory);
    uint8_t PullByteFromStack(Mem& memory);
    uint16_t PullWordFromStack(Mem& memory);

    // Operation functions
    // LOAD & STORE
    void OpLDA(uint16_t address, Mem& memory);
    void OpLDX(uint16_t address, Mem& memory);
    void OpLDY(uint16_t address, Mem& memory);
    void OpSTA(uint16_t address, Mem& memory);
    void OpSTX(uint16_t address, Mem& memory);
    void OpSTY(uint16_t address, Mem& memory);

    // REGISTER TRANSFERS
    void OpTAX(uint16_t address, Mem& memory);
    void OpTAY(uint16_t address, Mem& memory);
    void OpTXA(uint16_t address, Mem& memory);
    void OpTYA(uint16_t address, Mem& memory);

    // STACK OPERATIONS
    void OpTSX(uint16_t address, Mem& memory);
    void OpTXS(uint16_t address, Mem& memory);
    void OpPHA(uint16_t address, Mem& memory);
    void OpPHP(uint16_t address, Mem& memory);
    void OpPLA(uint16_t address, Mem& memory);
    void OpPLP(uint16_t address, Mem& memory);

    // LOGICAL OPERATIONS
    void OpAND(uint16_t address, Mem& memory);
    void OpEOR(uint16_t address, Mem& memory);
    void OpORA(uint16_t address, Mem& memory);
    void OpBIT(uint16_t address, Mem& memory);

    // ARITHMETIC OPERATIONS
    void OpADC(uint16_t address, Mem& memory);
    void OpSBC(uint16_t address, Mem& memory);
    void OpCMP(uint16_t address, Mem& memory);
    void OpCPX(uint16_t address, Mem& memory);
    void OpCPY(uint16_t address, Mem& memory);

    // INCREMENT & DECREMENT OPERATIONS
    void OpINC(uint16_t address, Mem& memory);
    void OpINX(uint16_t address, Mem& memory);
    void OpINY(uint16_t address, Mem& memory);

    void OpDEC(uint16_t address, Mem& memory);
    void OpDEX(uint16_t address, Mem& memory);
    void OpDEY(uint16_t address, Mem& memory);

    // SHIFT OPERATIONS
    void OpASLA(uint16_t address, Mem& memory);
    void OpASL(uint16_t address, Mem& memory);
    void OpLSRA(uint16_t address, Mem& memory);
    void OpLSR(uint16_t address, Mem& memory);
    void OpROLA(uint16_t address, Mem& memory);
    void OpROL(uint16_t address, Mem& memory);
    void OpRORA(uint16_t address, Mem& memory);
    void OpROR(uint16_t address, Mem& memory);

    // JUMPS & CALLS OPERATIONS
    void OpJMP(uint16_t address, Mem& memory);
    void OpJSR(uint16_t address, Mem& memory);
    void OpRTS(uint16_t address, Mem& memory);

    // BRANCH OPERATIONS
    void OpBCC(uint16_t address, Mem& memory);
    void OpBCS(uint16_t address, Mem& memory);
    void OpBEQ(uint16_t address, Mem& memory);
    void OpBMI(uint16_t address, Mem& memory);
    void OpBNE(uint16_t address, Mem& memory);
    void OpBPL(uint16_t address, Mem& memory);
    void OpBVC(uint16_t address, Mem& memory);
    void OpBVS(uint16_t address, Mem& memory);

    // STATUS FLAG OPERATIONS
    void OpCLC(uint16_t address, Mem& memory);
    void OpCLD(uint16_t address, Mem& memory);
    void OpCLI(uint16_t address, Mem& memory);
    void OpCLV(uint16_t address, Mem& memory);
    void OpSEC(uint16_t address, Mem& memory);
    void OpSED(uint16_t address, Mem& memory);
    void OpSEI(uint16_t address, Mem& memory);

    // SYSTEM OPERATIONS
    void OpBRK(uint16_t address, Mem& memory);
    void OpNOP(uint16_t address, Mem& memory);
    void OpRTI(uint16_t address, Mem& memory);

    void OpIllegal(uint16_t address, Mem& memory);
};

#endif  // CPU_H