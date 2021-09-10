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

        reg = 0b10001000;

        // INLINE PROGRAM
        mem[0xFFFC] = opcode;

        const uint32_t cycles = 2;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(reg, 0b10001001);
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