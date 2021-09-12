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

#include <gtest/gtest.h>

#include "cpu.h"

class JumpsCallsTests : public ::testing::Test
{
   public:
    Mem mem;
    CPU cpu;

   protected:
    void SetUp() override
    {
        cpu.Reset(mem);
    }
};

// Tests for JMP

TEST_F(JumpsCallsTests, JMPAbsolute)
{
    mem[0xFFFC] = 0x4C;
    mem[0xFFFD] = 0x20;
    mem[0xFFFE] = 0x20;

    const uint32_t cycles = 3;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.PC, 0x2020);
    EXPECT_EQ(used_cycles, cycles);
}

TEST_F(JumpsCallsTests, JMPIndirect)
{
    mem[0xFFFC] = 0x6C;
    mem[0xFFFD] = 0x20;
    mem[0xFFFE] = 0x25;
    mem[0x2520] = 0x22;
    mem[0x2521] = 0x22;

    const uint32_t cycles = 5;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.PC, 0x2222);
    EXPECT_EQ(used_cycles, cycles);
}

TEST_F(JumpsCallsTests, JMPIndirectOriginalBug)
{
    mem[0x3000] = 0x40;

    mem[0xFFFC] = 0x6C;
    mem[0xFFFD] = 0xFF;
    mem[0xFFFE] = 0x30;
    mem[0x30FF] = 0x80;
    mem[0x3100] = 0x50;

    const uint32_t cycles = 5;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.PC, 0x4080);
    EXPECT_EQ(used_cycles, cycles);
}

// TEST FOR JSR

TEST_F(JumpsCallsTests, JSR)
{
    mem[0xFFFC] = 0x20;
    mem[0xFFFD] = 0x08;
    mem[0xFFFE] = 0x80;
    mem[0x8008] = 0x90;

    const uint32_t cycles = 6;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(mem[0x100 + cpu.SP + 1], 0xFE);
    EXPECT_EQ(mem[0x100 + cpu.SP + 2], 0xFF);
    EXPECT_EQ(used_cycles, cycles);
}

// TEST FOR RTS

TEST_F(JumpsCallsTests, RTS)
{
    // Put program counter 0x3035 into the stack
    cpu.SP -= 2;
    mem[0x100 + cpu.SP + 1] = 0x35;
    mem[0x100 + cpu.SP + 2] = 0x30;

    mem[0xFFFC] = 0x60;

    const uint32_t cycles = 6;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.PC, 0x3035);
    EXPECT_EQ(used_cycles, cycles);
}

// Testing JSR -> operation -> RTS

TEST_F(JumpsCallsTests, JSRAndRTS)
{
    cpu.X = 2;

    mem[0xFFFC] = 0x20;  // JSR
    mem[0xFFFD] = 0x05;
    mem[0xFFFE] = 0x03;
    mem[0x0305] = 0xE8;  // Increment cpu.X
    mem[0x0306] = 0x60;  // RTS

    const uint32_t cycles = 6 + 2 + 6;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.X, 2 + 1);
    EXPECT_EQ(cpu.PC, 0xFFFE);
    EXPECT_EQ(used_cycles, cycles);
}