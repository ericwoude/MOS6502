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

class SystemTests : public ::testing::Test
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

// Tests for BRK

TEST_F(SystemTests, BRK)
{
    mem[0xFFFC] = 0x00;
    mem[0xFFFE] = 0x20;
    mem[0xFFFF] = 0x30;

    const uint32_t cycles = 7;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.PC, 0x3020);
    EXPECT_EQ(cycles, used_cycles);
}

// Tests for NOP

TEST_F(SystemTests, NOP)
{
    mem[0xFFFC] = 0xEA;

    const uint32_t cycles = 2;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cycles, used_cycles);
    EXPECT_EQ(cpu.PC, 0xFFFD);
}

// Tests for RTI

TEST_F(SystemTests, RTI)
{
    cpu.C = 1;
    cpu.Z = 1;
    cpu.I = 1;
    cpu.D = 1;
    cpu.B = 1;
    cpu.V = 1;
    cpu.N = 1;

    cpu.PC = 0xFFF0;
    mem[0xFFF0] = 0x00;  // BRK
    mem[0xFFF1] = 0x20;
    mem[0xFFF2] = 0x30;
    mem[0x3020] = 0x40;  // RTI
    mem[0xFFFE] = 0x45;
    mem[0xFFFF] = 0x01;

    const uint32_t cycles = 7 + 6;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cycles, used_cycles);
    EXPECT_EQ(cpu.PC, 0xFFF1);

    EXPECT_TRUE(cpu.C);
    EXPECT_TRUE(cpu.Z);
    EXPECT_TRUE(cpu.I);
    EXPECT_TRUE(cpu.D);
    EXPECT_TRUE(cpu.B);
    EXPECT_TRUE(cpu.V);
    EXPECT_TRUE(cpu.N);
}