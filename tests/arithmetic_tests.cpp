#include <gtest/gtest.h>

#include "cpu.h"

class ArithmeticTests : public ::testing::Test
{
    public:
    Mem mem;
    CPU cpu;

    protected:
    void SetUp() override
    {
        cpu.Reset(mem);
    }

    struct TestVariables
    {
        uint8_t A;
        uint8_t M; // Memory content, will be added to A.
        uint8_t result;

        bool carry; // Do we have a carry bit
        
        bool C, Z, V, N; // Processor status
    };

    TestVariables standardTest =
    {
        standardTest.A = 13,
        standardTest.M = 12,
        standardTest.result = 26,
        standardTest.carry = false,
        standardTest.C = false,
        standardTest.Z = false,
        standardTest.V = false,
        standardTest.N
    };


    void TestADCImmediate(TestVariables test)
    {
        cpu.A = test.A;

        cpu.C = test.carry;
        cpu.Z = !test.Z;
        cpu.V = !test.V;
        cpu.N = !test.N;

        mem[0xFFFC] = 0x69;
        mem[0xFFFD] = test.M;

        const uint32_t cycles = 2;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(cpu.A, test.result);
        EXPECT_EQ(cycles, used_cycles);

        EXPECT_EQ(cpu.C, test.C);
        EXPECT_EQ(cpu.Z, test.Z);
        EXPECT_EQ(cpu.V, test.V);
        EXPECT_EQ(cpu.N, test.N);
    }
};

// Tests for ADC

TEST_F(ArithmeticTests, ADCAddPositiveToPositive)
{
    TestVariables test;
    test.A = 1;
    test.M = 1;
    test.result = 2;
    test.carry = false;
    test.C = false;
    test.Z = false;
    test.V = false;
    test.N = false;

    TestADCImmediate(test);
}

TEST_F(ArithmeticTests, ADCAddCarry)
{
    TestVariables test;
    test.A = 0;
    test.M = 0;
    test.result = 1;
    test.carry = true;
    test.C = false;
    test.Z = false;
    test.V = false;
    test.N = false;

    TestADCImmediate(test);
}

TEST_F(ArithmeticTests, ADCAddOverflow)
{
    TestVariables test;
    test.A = 0b11111111;
    test.M = 0b00000001;
    test.result = 0b00000000;
    test.carry = false;
    test.C = true;
    test.Z = true;
    test.V = true;
    test.N = false;

    TestADCImmediate(test);
}

TEST_F(ArithmeticTests, ADCAddOverflowCarry)
{
    TestVariables test;
    test.A = 0b11111111;
    test.M = 0b00000001;
    test.result = 0b00000001;
    test.carry = true;
    test.C = true;
    test.Z = false;
    test.V = true;
    test.N = false;

    TestADCImmediate(test);
}

TEST_F(ArithmeticTests, ADCAddNegativeToPositive)
{
    TestVariables test;
    test.A = 13;
    test.M = (uint8_t) -3;
    test.result = 11;
    test.carry = true;
    test.C = true;
    test.Z = false;
    test.V = false;
    test.N = false;

    TestADCImmediate(test);
}