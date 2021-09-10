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

class IncrementsDecrementsTest : public ::testing::Test
{
   public:
    Mem mem;
    CPU cpu;

   protected:
    void SetUp() override
    {
        cpu.Reset(mem);
    }

    void TestIncrementZero(uint8_t opcode, uint8_t& reg)
    {
        cpu.Z = true;
        cpu.N = true;

        reg = 0;

        // INLINE PROGRAM
        mem[0xFFFC] = opcode;

        const uint32_t cycles = 2;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(reg, 1);
        EXPECT_EQ(cycles, used_cycles);

        EXPECT_FALSE(cpu.Z);
        EXPECT_FALSE(cpu.N);
    }

    void TestIncrementOverflow(uint8_t opcode, uint8_t& reg)
    {
        cpu.Z = false;
        cpu.N = true;

        reg = 0xFF;

        // INLINE PROGRAM
        mem[0xFFFC] = opcode;

        const uint32_t cycles = 2;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(reg, 0);
        EXPECT_EQ(cycles, used_cycles);

        EXPECT_TRUE(cpu.Z);
        EXPECT_FALSE(cpu.N);
    }

    void TestIncrementNegative(uint8_t opcode, uint8_t& reg)
    {
        cpu.Z = true;
        cpu.N = false;

        reg = 0b10001110;

        // INLINE PROGRAM
        mem[0xFFFC] = opcode;

        const uint32_t cycles = 2;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(reg, 0b10001111);
        EXPECT_EQ(cycles, used_cycles);

        EXPECT_FALSE(cpu.Z);
        EXPECT_TRUE(cpu.N);
    }

    void TestDecrementOne(uint8_t opcode, uint8_t& reg)
    {
        cpu.Z = true;
        cpu.N = true;

        reg = 1;

        // INLINE PROGRAM
        mem[0xFFFC] = opcode;

        const uint32_t cycles = 2;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(reg, 0);
        EXPECT_EQ(cycles, used_cycles);

        EXPECT_TRUE(cpu.Z);
        EXPECT_FALSE(cpu.N);
    }

    void TestDecrementOverflow(uint8_t opcode, uint8_t& reg)
    {
        cpu.Z = true;
        cpu.N = false;

        reg = 0;

        // INLINE PROGRAM
        mem[0xFFFC] = opcode;

        const uint32_t cycles = 2;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(reg, 0xFF);
        EXPECT_EQ(cycles, used_cycles);

        EXPECT_FALSE(cpu.Z);
        EXPECT_TRUE(cpu.N);
    }

    void TestDecrementNegative(uint8_t opcode, uint8_t& reg)
    {
        cpu.Z = true;
        cpu.N = false;

        reg = 0b10001111;

        // INLINE PROGRAM
        mem[0xFFFC] = opcode;

        const uint32_t cycles = 2;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(reg, 0b10001110);
        EXPECT_EQ(cycles, used_cycles);

        EXPECT_FALSE(cpu.Z);
        EXPECT_TRUE(cpu.N);
    }
};

TEST_F(IncrementsDecrementsTest, INCSimple)
{
    cpu.Z = true;
    cpu.N = true;

    // INLINE PROGRAM
    mem[0xFFFC] = 0xE6;
    mem[0xFFFD] = 0x22;
    mem[0x0022] = 2;

    const uint32_t cycles = 5;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(mem[0x0022], 3);
    EXPECT_EQ(cycles, used_cycles);

    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(IncrementsDecrementsTest, INCOverflow)
{
    cpu.Z = false;
    cpu.N = true;

    // INLINE PROGRAM
    mem[0xFFFC] = 0xE6;
    mem[0xFFFD] = 0x22;
    mem[0x0022] = 0xFF;

    const uint32_t cycles = 5;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(mem[0x0022], 0);
    EXPECT_EQ(cycles, used_cycles);

    EXPECT_TRUE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

// Tests for INX

TEST_F(IncrementsDecrementsTest, INXZero)
{
    TestIncrementZero(0xE8, cpu.X);
}

TEST_F(IncrementsDecrementsTest, INXOverflow)
{
    TestIncrementOverflow(0xE8, cpu.X);
}

TEST_F(IncrementsDecrementsTest, INXNegative)
{
    TestIncrementNegative(0xE8, cpu.X);
}

// Tests for INY

TEST_F(IncrementsDecrementsTest, INYZero)
{
    TestIncrementZero(0xC8, cpu.Y);
}

TEST_F(IncrementsDecrementsTest, INYOverflow)
{
    TestIncrementOverflow(0xC8, cpu.Y);
}

TEST_F(IncrementsDecrementsTest, INYNegative)
{
    TestIncrementNegative(0xC8, cpu.Y);
}

// TESTS FOR DEC

TEST_F(IncrementsDecrementsTest, DECSimple)
{
    cpu.Z = true;
    cpu.N = true;

    // INLINE PROGRAM
    mem[0xFFFC] = 0xC6;
    mem[0xFFFD] = 0x22;
    mem[0x0022] = 2;

    const uint32_t cycles = 5;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(mem[0x0022], 1);
    EXPECT_EQ(cycles, used_cycles);

    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(IncrementsDecrementsTest, DECOverflow)
{
    cpu.Z = true;
    cpu.N = false;

    // INLINE PROGRAM
    mem[0xFFFC] = 0xC6;
    mem[0xFFFD] = 0x22;
    mem[0x0022] = 0;

    const uint32_t cycles = 5;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(mem[0x0022], 0xFF);
    EXPECT_EQ(cycles, used_cycles);

    EXPECT_FALSE(cpu.Z);
    EXPECT_TRUE(cpu.N);
}

// Tests for DEX

TEST_F(IncrementsDecrementsTest, DEXOne)
{
    TestDecrementOne(0xCA, cpu.X);
}

TEST_F(IncrementsDecrementsTest, DEXOverflow)
{
    TestDecrementOverflow(0xCA, cpu.X);
}

TEST_F(IncrementsDecrementsTest, DEXNegative)
{
    TestDecrementNegative(0xCA, cpu.X);
}

// Tests for INY

TEST_F(IncrementsDecrementsTest, DEYONE)
{
    TestDecrementOne(0x88, cpu.Y);
}

TEST_F(IncrementsDecrementsTest, DEYOverflow)
{
    TestDecrementOverflow(0x88, cpu.Y);
}

TEST_F(IncrementsDecrementsTest, DEYNegative)
{
    TestDecrementNegative(0x88, cpu.Y);
}