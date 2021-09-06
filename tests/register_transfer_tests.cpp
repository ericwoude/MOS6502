#include <gtest/gtest.h>

#include "cpu.h"

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
        
        const uint32_t cycles = 2;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(from, to);
        EXPECT_EQ(cycles, used_cycles);
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