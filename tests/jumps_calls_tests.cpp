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
    // EXPECT_NE(cpu.PC, 0x2222);
}