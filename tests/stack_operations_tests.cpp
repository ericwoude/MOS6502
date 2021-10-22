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

class StackOperationTests : public ::testing::Test
{
   public:
    Mem mem;
    CPU cpu;

   protected:
    void SetUp() override
    {
        cpu.Reset(mem);
    }

    void TestPushOnStack(uint8_t opcode, uint8_t& reg)
    {
        cpu.SP = 0xF0;
        reg = 0x02;

        // INLINE PROGRAM
        mem[0xFFFC] = opcode;

        const uint32_t cycles = 3;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        // Check if the register content is pushed on the
        // stack and if the stack pointer is decremented.
        EXPECT_EQ(mem[0x100 + cpu.SP + 1], reg);
        EXPECT_EQ(cpu.SP, 0xF0 - 1);
        EXPECT_EQ(cycles, used_cycles);
    }

    void TestPullFromStack(uint8_t opcode, uint8_t& reg)
    {
        cpu.SP = 0xFE;

        // INLINE PROGRAM
        mem[0xFFFC] = opcode;
        mem[0x01FF] = 0b11111111;

        const uint32_t cycles = 4;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        // Check if the value is pulled from stack into
        // the register and if the stack pointer is incremented.
        EXPECT_EQ(reg, 0b11111111);
        EXPECT_EQ(cpu.SP, 0xFF);
        EXPECT_EQ(cycles, used_cycles);
    }
};

TEST_F(StackOperationTests, TSX)
{
    cpu.SP = 0x0A;

    // INLINE PROGRAM
    mem[0xFFFC] = 0xBA;

    const uint32_t cycles = 2;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.X, cpu.SP);
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(StackOperationTests, TXS)
{
    cpu.X = 0x0A;

    // INLINE PROGRAM
    mem[0xFFFC] = 0x9A;

    const uint32_t cycles = 2;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.SP, cpu.X);
    EXPECT_EQ(cycles, used_cycles);
}

TEST_F(StackOperationTests, PHA)
{
    TestPushOnStack(0x48, cpu.A);
}

TEST_F(StackOperationTests, PHP)
{
    // TestPushOnStack(0x08, cpu.PS);
    cpu.SP = 0xF0;
    cpu.PS = 0x00;

    // INLINE PROGRAM
    mem[0xFFFC] = 0x08;

    const uint32_t cycles = 3;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    // Check if the register content is pushed on the
    // stack and if the stack pointer is decremented.
    // The PHP operation sets the 5th and 4th bit
    // before pushing the processor status.
    EXPECT_EQ(mem[0x100 + cpu.SP + 1], cpu.PS | 0b00110000);
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_EQ(cpu.SP, 0xF0 - 1);
}

TEST_F(StackOperationTests, PLA)
{
    TestPullFromStack(0x68, cpu.A);

    // The N, Z flags are set accordingly.
    EXPECT_FALSE(cpu.Z);
    EXPECT_TRUE(cpu.N);
}

TEST_F(StackOperationTests, PLP)
{
    cpu.SP = 0xFE;

    // INLINE PROGRAM
    mem[0xFFFC] = 0x28;
    mem[0x01FF] = 0b11001111;

    const uint32_t cycles = 4;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    // Check if the value is pulled from stack into
    // the register and if the stack pointer is incremented.
    EXPECT_EQ(cpu.PS, 0b11001111);
    EXPECT_EQ(cpu.SP, 0xFF);
    EXPECT_EQ(cycles, used_cycles);
}