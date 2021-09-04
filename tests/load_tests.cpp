#include <gtest/gtest.h>

#include "MOS6502.h"

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

    cpu.Execute(2, mem);

    EXPECT_EQ(cpu.A, 0x22);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadTests, LDAZeroPage)
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

TEST_F(LoadTests, LDAZeroPageX)
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

TEST_F(LoadTests, LDAZeroPageXWrapAround)
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

TEST_F(LoadTests, LDAAbsolute)
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

TEST_F(LoadTests, LDAAbsoluteX)
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

TEST_F(LoadTests, LDAAbsoluteY)
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

TEST_F(LoadTests, LDAIndirectX)
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

TEST_F(LoadTests, LDAIndirectY)
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

// Tests for LDX instructions

TEST_F(LoadTests, LDXImmediate)
{
    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDX_IM;
    mem[0xFFFD] = 0x22;

    cpu.Execute(2, mem);

    EXPECT_EQ(cpu.X, 0x22);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadTests, LDXZeroPage)
{
    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDX_ZP;
    mem[0xFFFD] = 0x15;
    mem[0x0015] = 0x22;

    cpu.Execute(3, mem);

    EXPECT_EQ(cpu.X, 0x22);
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
    
    cpu.Execute(4, mem);

    EXPECT_EQ(cpu.X, 0x22);
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

    cpu.Execute(4, mem);

    EXPECT_EQ(cpu.X, 0x22);
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
    
    cpu.Execute(4, mem);

    EXPECT_EQ(cpu.X, 0x22);
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
    
    cpu.Execute(4, mem);

    EXPECT_EQ(cpu.X, 0x22);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

// Tests for LDY instructions

TEST_F(LoadTests, LDYImmediate)
{
    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDY_IM;
    mem[0xFFFD] = 0x22;

    cpu.Execute(2, mem);

    EXPECT_EQ(cpu.Y, 0x22);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(LoadTests, LDYZeroPage)
{
    // INLINE PROGRAM
    mem[0xFFFC] = CPU::LDY_ZP;
    mem[0xFFFD] = 0x15;
    mem[0x0015] = 0x22;

    cpu.Execute(3, mem);

    EXPECT_EQ(cpu.Y, 0x22);
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
    
    cpu.Execute(4, mem);

    EXPECT_EQ(cpu.Y, 0x22);
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
    
    cpu.Execute(4, mem);

    EXPECT_EQ(cpu.Y, 0x22);
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
    
    cpu.Execute(4, mem);

    EXPECT_EQ(cpu.Y, 0x22);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}
