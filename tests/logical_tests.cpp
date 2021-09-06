#include <gtest/gtest.h>

#include "cpu.h"

class LogicalTests : public ::testing::Test
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

TEST_F(LogicalTests, ANDImmediate)
{
    cpu.A = 0b10101010;

    mem[0xFFFC] = 0x29; // AND Immediate opcode
    mem[0xFFFD] = 0b00001000;

    const uint32_t cycles = 2;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0b00001000); // <= 0b10101010 & 0b00001000
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LogicalTests, ANDZeroPage)
{
    cpu.A = 0b10101010;

    mem[0xFFFC] = 0x25; // AND ZeroPage opcode
    mem[0xFFFD] = 0x05;
    mem[0x0005] = 0b00001000;

    const uint32_t cycles = 3;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0b00001000); // <= 0b10101010 & 0b00001000
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LogicalTests, ANDZeroPageX)
{
    cpu.X = 0x04;
    cpu.A = 0b10101010;

    mem[0xFFFC] = 0x35; // AND ZeroPageX opcode
    mem[0xFFFD] = 0x05;
    mem[0x0009] = 0b00001000;

    const uint32_t cycles = 4;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0b00001000); // <= 0b10101010 & 0b00001000
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LogicalTests, ANDAbsolute)
{
    cpu.X = 0x04;
    cpu.A = 0b10101010;

    mem[0xFFFC] = 0x2D; // AND Absolute opcode
    mem[0xFFFD] = 0x05;
    mem[0xFFFE] = 0x05;
    mem[0x0505] = 0b00001000;

    const uint32_t cycles = 4;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0b00001000); // <= 0b10101010 & 0b00001000
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LogicalTests, ANDAbsoluteX)
{
    cpu.X = 0x04;
    cpu.A = 0b10101010;

    mem[0xFFFC] = 0x3D; // AND AbsoluteX opcode
    mem[0xFFFD] = 0x05;
    mem[0xFFFE] = 0x05;
    mem[0x0509] = 0b00001000;

    const uint32_t cycles = 4;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0b00001000); // <= 0b10101010 & 0b00001000
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LogicalTests, ANDAbsoluteXPageCrossed)
{
    cpu.X = 0xFF;
    cpu.A = 0b10101010;

    mem[0xFFFC] = 0x3D; // AND AbsoluteX opcode
    mem[0xFFFD] = 0x01;
    mem[0xFFFE] = 0x05;
    mem[0x0600] = 0b00001000; // Crossed page boundary

    const uint32_t cycles = 5;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0b00001000); // <= 0b10101010 & 0b00001000
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LogicalTests, ANDAbsoluteY)
{
    cpu.Y = 0x04;
    cpu.A = 0b10101010;

    mem[0xFFFC] = 0x39; // AND AbsoluteY opcode
    mem[0xFFFD] = 0x05;
    mem[0xFFFE] = 0x05;
    mem[0x0509] = 0b00001000;

    const uint32_t cycles = 4;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0b00001000); // <= 0b10101010 & 0b00001000
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LogicalTests, ANDAbsoluteYPageCrossed)
{
    cpu.Y = 0xFF;
    cpu.A = 0b10101010;

    mem[0xFFFC] = 0x39; // AND AbsoluteX opcode
    mem[0xFFFD] = 0x01;
    mem[0xFFFE] = 0x05;
    mem[0x0600] = 0b00001000; // Crossed page boundary

    const uint32_t cycles = 5;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0b00001000); // <= 0b10101010 & 0b00001000
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LogicalTests, ANDIndexedIndirect)
{
    cpu.X = 0x02;
    cpu.A = 0b10101010;

    mem[0xFFFC] = 0x21; // AND IndexedIndirect opcode
    mem[0xFFFD] = 0x05;
    mem[0x0007] = 0x0A;
    mem[0x0008] = 0x0A;
    mem[0x0A0A] = 0b00001000;

    const uint32_t cycles = 6;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0b00001000); // <= 0b10101010 & 0b00001000
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);


}

TEST_F(LogicalTests, ANDIndexedIndirectWrapAround)
{
    cpu.X = 0xFF;
    cpu.A = 0b10101010;

    mem[0xFFFC] = 0x21; // AND IndexedIndirect opcode
    mem[0xFFFD] = 0x01;
    mem[0x0000] = 0x0A; // (0x01 + 0xFF) & 0xFF = 0x00
    mem[0x0001] = 0x0A;
    mem[0x0A0A] = 0b00001000;

    const uint32_t cycles = 6;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0b00001000); // <= 0b10101010 & 0b00001000
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LogicalTests, ANDIndirectIndexed)
{
    cpu.Y = 0x02;
    cpu.A = 0b10101010;

    mem[0xFFFC] = 0x31; // AND IndexedIndirect opcode
    mem[0xFFFD] = 0x02;
    mem[0x0002] = 0x00;
    mem[0x0003] = 0x80;
    mem[0x8002] = 0b00001000;

    const uint32_t cycles = 5;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0b00001000); // <= 0b10101010 & 0b00001000
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LogicalTests, ANDIndirectIndexedPageCrossed)
{
    cpu.Y = 0xFF;
    cpu.A = 0b10101010;

    mem[0xFFFC] = 0x31; // AND IndexedIndirect opcode
    mem[0xFFFD] = 0x02;
    mem[0x0002] = 0x01;
    mem[0x0003] = 0x05;
    mem[0x0600] = 0b00001000; // Crossed page boundary

    const uint32_t cycles = 6;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.A, 0b00001000); // <= 0b10101010 & 0b00001000
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}