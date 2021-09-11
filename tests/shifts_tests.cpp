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

class ShiftsTests : public ::testing::Test
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

// Tests for ASL Accumulator

TEST_F(ShiftsTests, ASLAccumulatorZero)
{
    cpu.A = 0;

    cpu.C = true;
    cpu.Z = false;
    cpu.N = true;

    // INLINE PROGRAM
    mem[0xFFFC] = 0x0A;

    const uint32_t cycles = 2;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0);
    EXPECT_EQ(cycles, used_cycles);

    EXPECT_FALSE(cpu.C);
    EXPECT_TRUE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(ShiftsTests, ASLAccumulatorOne)
{
    cpu.A = 1;

    cpu.C = true;
    cpu.Z = true;
    cpu.N = true;

    // INLINE PROGRAM
    mem[0xFFFC] = 0x0A;

    const uint32_t cycles = 2;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 2);
    EXPECT_EQ(cycles, used_cycles);

    EXPECT_FALSE(cpu.C);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(ShiftsTests, ASLAccumulatorCarry)
{
    cpu.A = 0b11111111;

    cpu.C = false;
    cpu.Z = true;
    cpu.N = false;

    // INLINE PROGRAM
    mem[0xFFFC] = 0x0A;

    const uint32_t cycles = 2;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0b11111110);
    EXPECT_EQ(cycles, used_cycles);

    EXPECT_TRUE(cpu.C);
    EXPECT_FALSE(cpu.Z);
    EXPECT_TRUE(cpu.N);
}

// Tests for ASL

TEST_F(ShiftsTests, ASLZero)
{
    cpu.C = true;
    cpu.Z = false;
    cpu.N = true;

    // INLINE PROGRAM
    mem[0xFFFC] = 0x06;
    mem[0xFFFD] = 0x22;
    mem[0x0022] = 0;

    const uint32_t cycles = 5;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(mem[0x0022], 0);
    EXPECT_EQ(cycles, used_cycles);

    EXPECT_FALSE(cpu.C);
    EXPECT_TRUE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(ShiftsTests, ASLOne)
{
    cpu.C = true;
    cpu.Z = true;
    cpu.N = true;

    // INLINE PROGRAM
    mem[0xFFFC] = 0x06;
    mem[0xFFFD] = 0x22;
    mem[0x0022] = 1;

    const uint32_t cycles = 5;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(mem[0x0022], 2);
    EXPECT_EQ(cycles, used_cycles);

    EXPECT_FALSE(cpu.C);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(ShiftsTests, ASLCarry)
{
    cpu.C = false;
    cpu.Z = true;
    cpu.N = false;

    // INLINE PROGRAM
    mem[0xFFFC] = 0x06;
    mem[0xFFFD] = 0x22;
    mem[0x0022] = 0b11111111;

    const uint32_t cycles = 5;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(mem[0x0022], 0b11111110);
    EXPECT_EQ(cycles, used_cycles);

    EXPECT_TRUE(cpu.C);
    EXPECT_FALSE(cpu.Z);
    EXPECT_TRUE(cpu.N);
}

// Tests for LSR Accumulator

TEST_F(ShiftsTests, LSRAccumulatorZero)
{
    cpu.A = 0;

    cpu.C = true;
    cpu.Z = false;
    cpu.N = true;

    // INLINE PROGRAM
    mem[0xFFFC] = 0x4A;

    const uint32_t cycles = 2;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0);
    EXPECT_EQ(cycles, used_cycles);

    EXPECT_FALSE(cpu.C);
    EXPECT_TRUE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(ShiftsTests, LSRAccumulatorCarry)
{
    cpu.A = 1;

    cpu.C = false;
    cpu.Z = false;
    cpu.N = true;

    // INLINE PROGRAM
    mem[0xFFFC] = 0x4A;

    const uint32_t cycles = 2;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0);
    EXPECT_EQ(cycles, used_cycles);

    EXPECT_TRUE(cpu.C);
    EXPECT_TRUE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

// Tests for LSR

TEST_F(ShiftsTests, LSRZero)
{
    cpu.C = true;
    cpu.Z = false;
    cpu.N = true;

    // INLINE PROGRAM
    mem[0xFFFC] = 0x46;
    mem[0xFFFD] = 0x22;
    mem[0x0022] = 0;

    const uint32_t cycles = 5;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(mem[0x0022], 0);
    EXPECT_EQ(cycles, used_cycles);

    EXPECT_FALSE(cpu.C);
    EXPECT_TRUE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(ShiftsTests, LSRCarry)
{
    cpu.C = false;
    cpu.Z = false;
    cpu.N = true;

    // INLINE PROGRAM
    mem[0xFFFC] = 0x46;
    mem[0xFFFD] = 0x22;
    mem[0x0022] = 1;

    const uint32_t cycles = 5;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(mem[0x0022], 0);
    EXPECT_EQ(cycles, used_cycles);

    EXPECT_TRUE(cpu.C);
    EXPECT_TRUE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}