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

class LoadTests : public ::testing::Test
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

// Tests for LDA instructions

TEST_F(LoadTests, LDAImmediate)
{
    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDA_IM;
    mem[0xFFFD] = 0x22;

    const uint32_t cycles = 2;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0x22);
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadTests, LDAZeroPage)
{
    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDA_ZP;
    mem[0xFFFD] = 0x15;
    mem[0x0015] = 0x22;

    const uint32_t cycles = 3;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0x22);
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadTests, LDAZeroPageX)
{
    cpu.X = 0xA;

    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDA_ZPX;
    mem[0xFFFD] = 0x05;
    mem[0x000F] = 0x22;

    const uint32_t cycles = 4;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0x22);
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadTests, LDAZeroPageXWrapAround)
{
    cpu.X = 0xFF;

    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDA_ZPX;
    mem[0xFFFD] = 0x80;
    mem[0x007F] = 0x22;

    const uint32_t cycles = 4;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0x22);
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadTests, LDAAbsolute)
{
    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDA_ABS;
    mem[0xFFFD] = 0x05;
    mem[0xFFFE] = 0x05;
    mem[0x0505] = 0x22;

    const uint32_t cycles = 4;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0x22);
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadTests, LDAAbsoluteX)
{
    cpu.X = 0x02;

    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDA_ABSX;
    mem[0xFFFD] = 0x05;
    mem[0xFFFE] = 0x05;
    mem[0x0507] = 0x22;

    const uint32_t cycles = 4;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0x22);
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadTests, LDAAbsoluteY)
{
    cpu.Y = 0x02;

    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDA_ABSY;
    mem[0xFFFD] = 0x05;
    mem[0xFFFE] = 0x05;
    mem[0x0507] = 0x22;

    const uint32_t cycles = 4;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0x22);
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadTests, LDAIndirectX)
{
    cpu.X = 0x02;

    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDA_INDX;
    mem[0xFFFD] = 0x05;
    mem[0x0007] = 0x0A;
    mem[0x0008] = 0x0A;
    mem[0x0A0A] = 0x22;

    const uint32_t cycles = 6;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0x22);
    EXPECT_FALSE(cpu.Z);
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadTests, LDAIndirectXWrapAround)
{
    cpu.X = 0xFF;

    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDA_INDX;
    mem[0xFFFD] = 0x01;
    mem[0x0000] = 0x0A;  // (0x01 + 0xFF) & 0xFF = 0x00
    mem[0x0001] = 0x0A;
    mem[0x0A0A] = 0x22;

    const uint32_t cycles = 6;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0x22);
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadTests, LDAIndirectY)
{
    cpu.Y = 0x04;

    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDA_INDY;
    mem[0xFFFD] = 0x02;
    mem[0x0002] = 0x00;
    mem[0x0003] = 0x80;
    mem[0x8004] = 0x22;

    const uint32_t cycles = 5;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0x22);
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadTests, LDAIndirectYPageCrossed)
{
    cpu.Y = 0xFF;

    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDA_INDY;
    mem[0xFFFD] = 0x02;
    mem[0x0002] = 0x01;
    mem[0x0003] = 0x0A;
    mem[0x0B00] = 0x22;  // 0x0A01 + 0xFF = 0x0B00

    const uint32_t cycles = 6;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0x22);
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

// Tests for LDX instructions

TEST_F(LoadTests, LDXImmediate)
{
    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDX_IM;
    mem[0xFFFD] = 0x22;

    const uint32_t cycles = 2;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.X, 0x22);
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadTests, LDXZeroPage)
{
    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDX_ZP;
    mem[0xFFFD] = 0x15;
    mem[0x0015] = 0x22;

    const uint32_t cycles = 3;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.X, 0x22);
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadTests, LDXZeroPageY)
{
    cpu.Y = 0xA;

    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDX_ZPY;
    mem[0xFFFD] = 0x05;
    mem[0x000F] = 0x22;

    const uint32_t cycles = 4;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.X, 0x22);
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadTests, LDXZeroPageYWrapAround)
{
    cpu.Y = 0xFF;

    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDX_ZPY;
    mem[0xFFFD] = 0x80;
    mem[0x007F] = 0x22;

    const uint32_t cycles = 4;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.X, 0x22);
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadTests, LDXAbsolute)
{
    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDX_ABS;
    mem[0xFFFD] = 0x05;
    mem[0xFFFE] = 0x05;
    mem[0x0505] = 0x22;

    const uint32_t cycles = 4;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.X, 0x22);
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadTests, LDXAbsoluteY)
{
    cpu.Y = 0x02;

    // // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDX_ABSY;
    mem[0xFFFD] = 0x01;
    mem[0xFFFE] = 0x01;
    mem[0x0103] = 0x22;

    const uint32_t cycles = 4;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.X, 0x22);
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

// Tests for LDY instructions

TEST_F(LoadTests, LDYImmediate)
{
    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDY_IM;
    mem[0xFFFD] = 0x22;

    const uint32_t cycles = 2;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.Y, 0x22);
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadTests, LDYZeroPage)
{
    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDY_ZP;
    mem[0xFFFD] = 0x15;
    mem[0x0015] = 0x22;

    const uint32_t cycles = 3;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.Y, 0x22);
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadTests, LDYZeroPageX)
{
    cpu.X = 0xA;

    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDY_ZPX;
    mem[0xFFFD] = 0x05;
    mem[0x000F] = 0x22;

    const uint32_t cycles = 4;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.Y, 0x22);
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadTests, LDYAbsolute)
{
    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDY_ABS;
    mem[0xFFFD] = 0x05;
    mem[0xFFFE] = 0x05;
    mem[0x0505] = 0x22;

    const uint32_t cycles = 4;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.Y, 0x22);
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadTests, LDYAbsoluteX)
{
    cpu.X = 0x02;

    // // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDY_ABSX;
    mem[0xFFFD] = 0x01;
    mem[0xFFFE] = 0x01;
    mem[0x0103] = 0x22;

    const uint32_t cycles = 4;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.Y, 0x22);
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}
