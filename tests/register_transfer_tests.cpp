#include <gtest/gtest.h>

#include "MOS6502.h"

class RegisterTransferTests : public ::testing::Test
{
    public:
    Mem mem;
    CPU cpu;

    protected:
    void SetUp() override
    {
        cpu.Reset(mem);
    }

    void TestTransferRegister(uint8_t opcode, uint8_t& from, uint8_t& to)
    {
        from = 0x22;

        // INLINE PROGRAM
        mem[0xFFFC] = opcode;
        
        cpu.Execute(2, mem);

        EXPECT_EQ(from, to);
        EXPECT_FALSE(cpu.Z);
        EXPECT_FALSE(cpu.N);
    }
};

TEST_F(RegisterTransferTests, TAX)
{
    TestTransferRegister(CPU::TAX, cpu.A, cpu.X);
}

TEST_F(RegisterTransferTests, TAY)
{
    TestTransferRegister(CPU::TAY, cpu.A, cpu.Y);
}

TEST_F(RegisterTransferTests, TXA)
{
    TestTransferRegister(CPU::TXA, cpu.X, cpu.A);
}

TEST_F(RegisterTransferTests, TYA)
{
    TestTransferRegister(CPU::TYA, cpu.Y, cpu.A);
}