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

class ArithmeticTests : public ::testing::Test
{
   public:
    Mem mem;
    CPU cpu;

   protected:
    void SetUp() override
    {
        cpu.Reset(mem);
    }

    struct TestVariables
    {
        uint8_t A;
        uint8_t M;  // Memory content, will be added to A.
        uint8_t result;

        bool carry;  // Do we have a carry bit

        bool C, Z, V, N;  // Processor status
    };

    TestVariables standardTest = {standardTest.A = 13,      standardTest.M = 12,
                                  standardTest.result = 26, standardTest.carry = false,
                                  standardTest.C = false,   standardTest.Z = false,
                                  standardTest.V = false,   standardTest.N};

    void TestImmediate(uint8_t opcode, TestVariables test)
    {
        cpu.A = test.A;

        cpu.C = test.carry;
        cpu.Z = !test.Z;
        cpu.V = !test.V;
        cpu.N = !test.N;

        mem[0xFFFC] = opcode;
        mem[0xFFFD] = test.M;

        const uint32_t cycles = 2;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(cpu.A, test.result);
        EXPECT_EQ(cycles, used_cycles);

        EXPECT_EQ(cpu.C, test.C);
        EXPECT_EQ(cpu.Z, test.Z);
        EXPECT_EQ(cpu.V, test.V);
        EXPECT_EQ(cpu.N, test.N);
    }

    void CMLessM(uint8_t opcode, uint8_t& reg)
    {
        cpu.C = true;
        cpu.Z = true;
        cpu.N = false;

        reg = 2;

        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 3;

        const uint32_t cycles = 2;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(cycles, used_cycles);

        EXPECT_FALSE(cpu.C);
        EXPECT_FALSE(cpu.Z);
        EXPECT_TRUE(cpu.N);
    }

    void CMEqualsM(uint8_t opcode, uint8_t& reg)
    {
        cpu.C = false;
        cpu.Z = false;
        cpu.N = true;

        reg = 55;

        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 55;

        const uint32_t cycles = 2;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(cycles, used_cycles);

        EXPECT_TRUE(cpu.C);
        EXPECT_TRUE(cpu.Z);
        EXPECT_FALSE(cpu.N);
    }

    void CMGreaterM(uint8_t opcode, uint8_t& reg)
    {
        cpu.C = false;
        cpu.Z = true;
        cpu.N = true;

        reg = 0x93;

        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x20;

        const uint32_t cycles = 2;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(cycles, used_cycles);

        EXPECT_TRUE(cpu.C);
        EXPECT_FALSE(cpu.Z);
        EXPECT_FALSE(cpu.N);
    }
};

// Tests for ADC

TEST_F(ArithmeticTests, ADCPositiveToPositive)
{
    TestVariables test;
    test.A = 1;
    test.M = 1;
    test.result = 2;
    test.carry = false;
    test.C = false;
    test.Z = false;
    test.V = false;
    test.N = false;

    TestImmediate(0x69, test);
}

TEST_F(ArithmeticTests, ADCCarry)
{
    TestVariables test;
    test.A = 0;
    test.M = 0;
    test.result = 1;
    test.carry = true;
    test.C = false;
    test.Z = false;
    test.V = false;
    test.N = false;

    TestImmediate(0x69, test);
}

TEST_F(ArithmeticTests, ADCOverflow)
{
    TestVariables test;
    test.carry = false;
    test.A = 127;
    test.M = 1;
    test.result = 128;
    test.C = false;
    test.N = true;
    test.V = true;
    test.Z = false;

    TestImmediate(0x69, test);
}

TEST_F(ArithmeticTests, ADCOverflowCarry)
{
    TestVariables test;
    test.carry = true;
    test.A = 127;
    test.M = 1;
    test.result = 129;
    test.C = false;
    test.N = true;
    test.V = true;
    test.Z = false;

    TestImmediate(0x69, test);
}

TEST_F(ArithmeticTests, ADCNegativeToPositive)
{
    TestVariables test;
    test.A = 13;
    test.M = (uint8_t)-3;
    test.result = 11;
    test.carry = true;
    test.C = true;
    test.Z = false;
    test.V = false;
    test.N = false;

    TestImmediate(0x69, test);
}

// Tests for SBC

TEST_F(ArithmeticTests, SBCZeroFromZero)
{
    TestVariables test;
    test.A = 0;
    test.M = 0;
    test.result = 0;
    test.carry = true;
    test.C = true;
    test.Z = true;
    test.V = false;
    test.N = false;

    TestImmediate(0xE9, test);
}

TEST_F(ArithmeticTests, SBCOneFromTwo)
{
    TestVariables test;
    test.A = 2;
    test.M = 1;
    test.result = 1;
    test.carry = true;
    test.C = true;
    test.Z = false;
    test.V = false;
    test.N = false;

    TestImmediate(0xE9, test);
}

TEST_F(ArithmeticTests, SBCOneFromZero)
{
    TestVariables test;
    test.A = 0;
    test.M = 1;
    test.result = (uint8_t)-1;
    test.carry = true;
    test.C = false;
    test.Z = false;
    test.V = false;
    test.N = true;

    TestImmediate(0xE9, test);
}

TEST_F(ArithmeticTests, SBCCarry)
{
    TestVariables test;
    test.A = 0;
    test.M = 1;
    test.result = (uint8_t)-2;
    test.carry = false;
    test.C = false;
    test.Z = false;
    test.V = false;
    test.N = true;

    TestImmediate(0xE9, test);
}

TEST_F(ArithmeticTests, SBCOverflow)
{
    TestVariables test;
    test.A = (uint8_t)-128;
    test.M = 1;
    test.result = 127;
    test.carry = true;
    test.C = true;
    test.Z = false;
    test.V = true;
    test.N = false;

    TestImmediate(0xE9, test);
}

TEST_F(ArithmeticTests, SBCOverflow2)
{
    TestVariables test;
    test.A = 127;
    test.M = (uint8_t)-1;
    test.result = 128;
    test.carry = true;
    test.C = false;
    test.Z = false;
    test.V = true;
    test.N = true;

    TestImmediate(0xE9, test);
}

// Tests for CMP

TEST_F(ArithmeticTests, CMPLessM)
{
    CMLessM(0xC9, cpu.A);
}

TEST_F(ArithmeticTests, CMPEqualsM)
{
    CMEqualsM(0xC9, cpu.A);
}

TEST_F(ArithmeticTests, CMPGreaterM)
{
    CMGreaterM(0xC9, cpu.A);
}

// Tests for CPX

TEST_F(ArithmeticTests, CPXLessM)
{
    CMLessM(0xE0, cpu.X);
}

TEST_F(ArithmeticTests, CPXEqualsM)
{
    CMEqualsM(0xE0, cpu.X);
}

TEST_F(ArithmeticTests, CPXGreaterM)
{
    CMGreaterM(0xE0, cpu.X);
}

// Tests for CPY

TEST_F(ArithmeticTests, CPYLessM)
{
    CMLessM(0xC0, cpu.Y);
}

TEST_F(ArithmeticTests, CPYEqualsM)
{
    CMEqualsM(0xC0, cpu.Y);
}

TEST_F(ArithmeticTests, CPYGreaterM)
{
    CMGreaterM(0xC0, cpu.Y);
}