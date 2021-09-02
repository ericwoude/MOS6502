#include <gtest/gtest.h>

#include "MOS6502.h"

class LoadStoreTests : public ::testing::Test
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

TEST_F(LoadStoreTests, LDAImmediate)
{
    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDA_IM;
    mem[0xFFFD] = 0x22;

    cpu.Execute(2, mem);

    EXPECT_EQ(cpu.A, 0x22);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadStoreTests, LDAZeroPage)
{
    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDA_ZP;
    mem[0xFFFD] = 0x15;
    mem[0x0015] = 0x22;

    cpu.Execute(3, mem);

    EXPECT_EQ(cpu.A, 0x22);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadStoreTests, LDAZeroPageX)
{
    cpu.X = 0xA;

    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDA_ZPX;
    mem[0xFFFD] = 0x05;
    mem[0x000F] = 0x22;
    
    cpu.Execute(4, mem);

    EXPECT_EQ(cpu.A, 0x22);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadStoreTests, LDAZeroPageXWrapAround)
{
    cpu.X = 0xFF;

    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDA_ZPX;
    mem[0xFFFD] = 0x80;
    mem[0x007F] = 0x22;

    cpu.Execute(4, mem);

    EXPECT_EQ(cpu.A, 0x22);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadStoreTests, LDAAbsolute)
{
    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDA_ABS;
    mem[0xFFFD] = 0x05;
    mem[0xFFFE] = 0x05;
    mem[0x0505] = 0x22;
    
    cpu.Execute(4, mem);

    EXPECT_EQ(cpu.A, 0x22);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadStoreTests, LDAAbsoluteX)
{
    cpu.X = 0x02;

    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDA_ABSX;
    mem[0xFFFD] = 0x05;
    mem[0xFFFE] = 0x05;
    mem[0x0507] = 0x22;
    
    cpu.Execute(4, mem);

    EXPECT_EQ(cpu.A, 0x22);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadStoreTests, LDAAbsoluteY)
{
    cpu.Y = 0x02;

    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDA_ABSY;
    mem[0xFFFD] = 0x05;
    mem[0xFFFE] = 0x05;
    mem[0x0507] = 0x22;
    
    cpu.Execute(4, mem);

    EXPECT_EQ(cpu.A, 0x22);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadStoreTests, LDAIndirectX)
{
    cpu.X = 0x02;

    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDA_INDX;
    mem[0xFFFD] = 0x05;
    mem[0x0007] = 0x0A;
    mem[0x0008] = 0x0A;
    mem[0x0A0A] = 0x22;

    cpu.Execute(6, mem);

    EXPECT_EQ(cpu.A, 0x22);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadStoreTests, LDAIndirectY)
{
    cpu.Y = 0x04;

    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDA_INDY;
    mem[0xFFFD] = 0x02;
    mem[0x0002] = 0x00;
    mem[0x0003] = 0x80;
    mem[0x8004] = 0x22;

    cpu.Execute(5, mem);

    EXPECT_EQ(cpu.A, 0x22);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}