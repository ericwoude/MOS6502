#include <gtest/gtest.h>

#include "cpu.h"

class StoreTests : public ::testing::Test
{
    public:
    Mem mem;
    CPU cpu;

    protected:
    void SetUp() override
    {
      cpu.Reset(mem);
    }

    // Tests for STAZeroPage, STXZeroPage, and STYZeroPage.
    void TestSTZeroPage(uint8_t opcode, uint8_t& reg)
    {
        reg = 0x09;

        // INLINE PROGRAM
        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x22;

        const uint32_t cycles = 3;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(mem[0x0022], reg);
        EXPECT_EQ(cycles, used_cycles);
    }

    // Tests for STAZeroPageX, STXZeroPageY, STYZeropageX.
    // Reg refers to the instruction register used,
    // variant refers to the addressing mode.
    void TestSTZeroPageXY(uint8_t opcode, uint8_t& reg, uint8_t& variant)
    {
        reg = 0x02;
        variant = 0x05;

        // INLINE PROGRAM
        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x20;
        mem[0x002 + variant] = 0x00;

        const uint32_t cycles = 4;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(mem[0x0020 + variant], reg);
        EXPECT_EQ(cycles, used_cycles);
    }

    // Tests for STAAbsolute, STXAbsolute, and STYAbsolute.
    void TestSTAbsolute(uint8_t opcode, uint8_t& reg)
    {
        reg = 0x09;

        // INLINE PROGRAM
        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x02;
        mem[0xFFFE] = 0x02;

        const uint32_t cycles = 4;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(mem[0x0202], reg);
        EXPECT_EQ(cycles, used_cycles);
    }

    // Tests for STAAbsoluteX and STAAbsoluteY.
    void TestSTAABsoluteXY(uint8_t opcode, uint8_t& reg)
    {
        cpu.A = 0x22;
        reg = 0x09;

        // INLINE PROGRAM
        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x02;
        mem[0xFFFE] = 0x02;

        const uint32_t cycles = 5;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(mem[0x0202 + reg], cpu.A);
        EXPECT_EQ(cycles, used_cycles);
    }
};

// Tests for STA instructions

TEST_F(StoreTests, STAZeroPage)
{
    TestSTZeroPage(CPU::STA_ZP, cpu.A);
}

TEST_F(StoreTests, STAZeroPageX)
{
    TestSTZeroPageXY(CPU::STA_ZPX, cpu.A, cpu.X);
}

TEST_F(StoreTests, STAAbsolute)
{
    TestSTAbsolute(CPU::STA_ABS, cpu.A);
}

TEST_F(StoreTests, STAAbsoluteX)
{
    TestSTAABsoluteXY(CPU::STA_ABSX, cpu.X);
}

TEST_F(StoreTests, STAAbsoluteY)
{
    TestSTAABsoluteXY(CPU::STA_ABSY, cpu.Y);
}

TEST_F(StoreTests, STAIndirectX) // with wrap
{
    cpu.A = 0x09;
    cpu.X = 0x02;

    // INLINE PROGRAM
    mem[0xFFFC] = CPU::STA_INDX;
    mem[0xFFFD] = 0x05;
    mem[0x0007] = 0x0A;
    mem[0x0008] = 0x0A;

    const uint32_t cycles = 6;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(mem[0x0A0A], cpu.A);
    EXPECT_EQ(cycles, used_cycles);
}

TEST_F(StoreTests, STAIndirectY) // with wrap
{
    cpu.A = 0x09;
    cpu.Y = 0x02;

    // INLINE PROGRAM
    mem[0xFFFC] = CPU::STA_INDY;
    mem[0xFFFD] = 0x05;
    mem[0x0005] = 0x0A;
    mem[0x0006] = 0x0A;

    const uint32_t cycles = 6;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(mem[0x0A0A + cpu.Y], cpu.A);
    EXPECT_EQ(cycles, used_cycles);
}

// Tests for STX instructions

TEST_F(StoreTests, STXZeroPage)
{
    TestSTZeroPage(CPU::STX_ZP, cpu.X);
}

TEST_F(StoreTests, STXZeroPageY)
{
    TestSTZeroPageXY(CPU::STX_ZPY, cpu.X, cpu.Y);
}

TEST_F(StoreTests, STXAbsolute)
{
    TestSTAbsolute(CPU::STX_ABS, cpu.X);
}


// Tests for STA instructions

TEST_F(StoreTests, STYZeroPage)
{
    TestSTZeroPage(CPU::STY_ZP, cpu.Y);
}

TEST_F(StoreTests, STYZeroPageX)
{
    TestSTZeroPageXY(CPU::STY_ZPX, cpu.Y, cpu.X);
}

TEST_F(StoreTests, STYAbsolute)
{
    TestSTAbsolute(CPU::STY_ABS, cpu.Y);
}