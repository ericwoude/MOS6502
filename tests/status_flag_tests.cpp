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

class StatusFlagTests : public ::testing::Test
{
   public:
    Mem mem;
    CPU cpu;

   protected:
    void SetUp() override
    {
        cpu.Reset(mem);
    }

    void SetFlag(uint8_t opcode)
    {
        switch (opcode)
        {
            case 0x18: cpu.C = 0; break;
            case 0xD8: cpu.D = 0; break;
            case 0x58: cpu.I = 0; break;
            case 0xB8: cpu.V = 0; break;
            case 0x38: cpu.V = 1; break;
            case 0xF8: cpu.D = 1; break;
            case 0x78: cpu.I = 1; break;
        }
    }

    bool GetFlag(uint8_t opcode)
    {
        switch (opcode)
        {
            case 0x18: return cpu.C;
            case 0xD8: return cpu.D;
            case 0x58: return cpu.I;
            case 0xB8: return cpu.V;
            case 0x38: return cpu.V;
            case 0xF8: return cpu.D;
            case 0x78: return cpu.I;
        }

        return false;
    }
    void TestFlag(uint8_t opcode, bool status)
    {
        SetFlag(opcode);

        mem[0xFFFC] = opcode;

        const uint32_t cycles = 2;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(GetFlag(opcode), status);
        EXPECT_EQ(cycles, used_cycles);
    }
};

TEST_F(StatusFlagTests, CLC)
{
    TestFlag(0x18, false);
}

TEST_F(StatusFlagTests, CLD)
{
    TestFlag(0xD8, false);
}

TEST_F(StatusFlagTests, CLI)
{
    TestFlag(0x58, false);
}

TEST_F(StatusFlagTests, CLV)
{
    TestFlag(0xB8, false);
}

TEST_F(StatusFlagTests, SEC)
{
    TestFlag(0x38, true);
}

TEST_F(StatusFlagTests, SED)
{
    TestFlag(0xF8, true);
}

TEST_F(StatusFlagTests, SEI)
{
    TestFlag(0x78, true);
}