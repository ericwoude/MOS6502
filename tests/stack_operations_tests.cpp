#include <gtest/gtest.h>

#include "cpu.h"

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

        const uint32_t cycles = 3;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        // Check if the register content is pushed on the
        // stack and if the stack pointer is decremented.
        EXPECT_EQ(mem[0x100 + cpu.SP + 1], reg);
        EXPECT_EQ(cpu.SP, 0xF0 - 1);
        EXPECT_EQ(cycles, used_cycles);
    }

    void TestPullFromStack(uint8_t opcode, uint8_t& reg)
    {
        cpu.SP = 0xFE;

        // INLINE PROGRAM
        mem[0xFFFC] = opcode;
        mem[0x01FF] = 0b11111111;

        const uint32_t cycles = 4;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        // Check if the value is pulled from stack into
        // the register and if the stack pointer is incremented.
        EXPECT_EQ(reg, 0b11111111);
        EXPECT_EQ(cpu.SP, 0xFF);
        EXPECT_EQ(cycles, used_cycles);
    }
};

TEST_F(StackOperationTests, TSX)
{
    cpu.SP = 0x0A;

    // INLINE PROGRAM
    mem[0xFFFC] = CPU::TSX;

    const uint32_t cycles = 2;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.X, cpu.SP);
    EXPECT_EQ(cycles, used_cycles);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(StackOperationTests, TXS)
{
    cpu.X = 0x0A;

    // INLINE PROGRAM
    mem[0xFFFC] = CPU::TXS;

    const uint32_t cycles = 2;
    uint32_t used_cycles = cpu.Execute(cycles, mem);

    EXPECT_EQ(cpu.SP, cpu.X);
    EXPECT_EQ(cycles, used_cycles);
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

    // The N, Z flags are set accordingly.
    EXPECT_FALSE(cpu.Z);
    EXPECT_TRUE(cpu.N);
}

TEST_F(StackOperationTests, PLP)
{
    // Will pull 0b11111111 from stack into PS
    TestPullFromStack(CPU::PLP, cpu.PS);

    // Test all flags are set accordingly.
    EXPECT_TRUE(cpu.C);
    EXPECT_TRUE(cpu.Z);
    EXPECT_TRUE(cpu.I);
    EXPECT_TRUE(cpu.D);
    EXPECT_TRUE(cpu.B);
    EXPECT_TRUE(cpu.V);
    EXPECT_TRUE(cpu.N);
}