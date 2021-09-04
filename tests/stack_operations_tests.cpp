#include <gtest/gtest.h>

#include "MOS6502.h"

class StackOperationTests : public ::testing::Test
{
    public:
    Mem mem;
    CPU cpu;

    protected:
    void SetUp() override
    {
        cpu.Reset(mem);
    }

    void TestPushOnStack(uint8_t opcode, uint8_t& reg)
    {
        cpu.SP = 0xF0;
        reg = 0x02;

        // INLINE PROGRAM
        mem[0xFFFC] = opcode;

        cpu.Execute(3, mem);

        // Check if the register content is pushed on the
        // stack and if the stack pointer is decremented.
        EXPECT_EQ(mem[0x100 + cpu.SP + 1], reg);
        EXPECT_EQ(cpu.SP, 0xF0 - 1);
    }

    void TestPullFromStack(uint8_t opcode, uint8_t& reg)
    {
        cpu.SP = 0xFE;

        // INLINE PROGRAM
        mem[0xFFFC] = opcode;
        mem[0x01FF] = 0x22;

        cpu.Execute(4, mem);

        // Check if the value is pulled from stack into
        // the register and if the stack pointer is incremented.
        EXPECT_EQ(reg, 0x22);
        EXPECT_EQ(cpu.SP, 0xFF);

        // The N, Z flags are set accordingly.
        EXPECT_FALSE(cpu.Z);
        EXPECT_FALSE(cpu.N);
    }
};

TEST_F(StackOperationTests, TSX)
{
    cpu.SP = 0x0A;

    // INLINE PROGRAM
    mem[0xFFFC] = CPU::TSX;

    cpu.Execute(2, mem);

    EXPECT_EQ(cpu.X, cpu.SP);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(StackOperationTests, TXS)
{
    cpu.X = 0x0A;

    // INLINE PROGRAM
    mem[0xFFFC] = CPU::TXS;

    cpu.Execute(2, mem);

    EXPECT_EQ(cpu.SP, cpu.X);
}

TEST_F(StackOperationTests, PHA)
{
    TestPushOnStack(CPU::PHA, cpu.A);
}

TEST_F(StackOperationTests, PHP)
{
    TestPushOnStack(CPU::PHP, cpu.PS);
}

TEST_F(StackOperationTests, PLA)
{
    TestPullFromStack(CPU::PLA, cpu.A);
}

TEST_F(StackOperationTests, PLP)
{
    TestPullFromStack(CPU::PLP, cpu.PS);
}