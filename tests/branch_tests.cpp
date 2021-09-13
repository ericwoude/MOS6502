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

class BranchTests : public ::testing::Test
{
   public:
    Mem mem;
    CPU cpu;

   protected:
    void SetUp() override
    {
        cpu.Reset(mem);
    }

    void SetFlags(uint8_t opcode)
    {
        switch (opcode)
        {
            case 0x90: cpu.C = 0; break;
            case 0xB0: cpu.C = 1; break;
            case 0xF0: cpu.Z = 1; break;
            case 0x30: cpu.N = 1; break;
            case 0xD0: cpu.Z = 0; break;
            case 0x10: cpu.N = 0; break;
            case 0x50: cpu.V = 0; break;
            case 0x70: cpu.V = 1; break;
        }
    }

    void TestForward(uint8_t opcode)
    {
        SetFlags(opcode);

        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 1;

        const uint32_t cycles = 3;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(cpu.PC, 0xFFFE + 1);
        EXPECT_EQ(cycles, used_cycles);
    }

    void TestBackward(uint8_t opcode)
    {
        SetFlags(opcode);

        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0b11111100;  // negative 4

        const uint32_t cycles = 3;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(cpu.PC, 0xFFFA);
        EXPECT_EQ(cycles, used_cycles);
    }

    void TestPageCrossed(uint8_t opcode)
    {
        SetFlags(opcode);

        cpu.PC = 0xFF00;
        mem[0xFF00] = opcode;
        mem[0xFF01] = 0b11111101;  // negative 3

        const uint32_t cycles = 4;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(cpu.PC, 0xFEFF);
        EXPECT_EQ(cycles, used_cycles);
    }
};

// Tests for BCC

TEST_F(BranchTests, BCCForward)
{
    TestForward(0x90);
}

TEST_F(BranchTests, BCCBackwards)
{
    TestBackward(0x90);
}

TEST_F(BranchTests, BCCPageCrossed)
{
    TestPageCrossed(0x90);
}

// Tests for BCS

TEST_F(BranchTests, BCSForward)
{
    TestForward(0xB0);
}

TEST_F(BranchTests, BCSBackwards)
{
    TestBackward(0xB0);
}

TEST_F(BranchTests, BCSPageCrossed)
{
    TestPageCrossed(0xB0);
}

// Tests for BEQ

TEST_F(BranchTests, BEQForward)
{
    TestForward(0xF0);
}

TEST_F(BranchTests, BEQBackwards)
{
    TestBackward(0xF0);
}

TEST_F(BranchTests, BEQPageCrossed)
{
    TestPageCrossed(0xF0);
}

// Tests for BMI

TEST_F(BranchTests, BMIForward)
{
    TestForward(0x30);
}

TEST_F(BranchTests, BMIBackwards)
{
    TestBackward(0x30);
}

TEST_F(BranchTests, BMIPageCrossed)
{
    TestPageCrossed(0x30);
}

// Tests for BNE

TEST_F(BranchTests, BNEForward)
{
    TestForward(0xD0);
}

TEST_F(BranchTests, BNEBackwards)
{
    TestBackward(0xD0);
}

TEST_F(BranchTests, BNEPageCrossed)
{
    TestPageCrossed(0xD0);
}

// Tests for BPL

TEST_F(BranchTests, BPLForward)
{
    TestForward(0x10);
}

TEST_F(BranchTests, BPLBackwards)
{
    TestBackward(0x10);
}

TEST_F(BranchTests, BPLPageCrossed)
{
    TestPageCrossed(0x10);
}

// Tests for BVC

TEST_F(BranchTests, BVCForward)
{
    TestForward(0x50);
}

TEST_F(BranchTests, BVCBackwards)
{
    TestBackward(0x50);
}

TEST_F(BranchTests, BVCPageCrossed)
{
    TestPageCrossed(0x50);
}

// Tests for BVS

TEST_F(BranchTests, BVSForward)
{
    TestForward(0x70);
}

TEST_F(BranchTests, BVSBackwards)
{
    TestBackward(0x70);
}

TEST_F(BranchTests, BVSPageCrossed)
{
    TestPageCrossed(0x70);
}