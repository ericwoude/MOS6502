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

    void TestADCAbsolute(TestVariables Test)
	{
		// given:
		cpu.Reset( mem );
		cpu.C = Test.carry;
		cpu.A = Test.A;
		cpu.Z = !Test.Z;
		cpu.N = !Test.N;
		cpu.V = !Test.V;
		mem[0xFFFC] = 0x6D;
		mem[0xFFFD] = 0x00;
		mem[0xFFFE] = 0x80;
		mem[0x8000] = Test.M;
		constexpr uint32_t EXPECTED_CYCLES = 4;

		// when:
		const uint32_t ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

		// then:
		EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
		EXPECT_EQ( cpu.A, Test.result );
		EXPECT_EQ( cpu.C, Test.C );
		EXPECT_EQ( cpu.Z, Test.Z );
		EXPECT_EQ( cpu.N, Test.N );
		EXPECT_EQ( cpu.V, Test.V );
	}


    void TestImmediate(uint8_t opcode, TestVariables test)
    {
        cpu.A = test.A;

        cpu.C = test.carry;
        cpu.Z = !test.Z;
        cpu.V = !test.V;
        cpu.N = !test.N;

        mem[0xFFFC] = opcode;
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

TEST_F(ArithmeticTests, ADCPositiveToPositive)
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

    TestImmediate(0x69, test);
}

TEST_F(ArithmeticTests, ADCCarry)
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

    TestImmediate(0x69, test);
}

TEST_F(ArithmeticTests, ADCOverflow)
{
    TestVariables test;
	test.carry = false;
	test.A = 127;
	test.M = 1;
	test.result = 128;
	test.C = false;
	test.N = true;
	test.V = true;
	test.Z = false;

	TestImmediate(0x69, test);
}

TEST_F(ArithmeticTests, ADCOverflowCarry)
{
    TestVariables test;
	test.carry = true;
	test.A = 127;
	test.M = 1;
	test.result = 129;
	test.C = false;
	test.N = true;
	test.V = true;
	test.Z = false;

    TestImmediate(0x69, test);
}

TEST_F(ArithmeticTests, ADCNegativeToPositive)
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

    TestImmediate(0x69, test);
}

// Tests for SBC

TEST_F(ArithmeticTests, SBCZeroFromZero)
{
    TestVariables test;
    test.A = 0;
    test.M = 0;
    test.result = 0;
    test.carry = true;
    test.C = true;
    test.Z = true;
    test.V = false;
    test.N = false;

    TestImmediate(0xE9, test);
}


TEST_F(ArithmeticTests, SBCOneFromTwo)
{
    TestVariables test;
    test.A = 2;
    test.M = 1;
    test.result = 1;
    test.carry = true;
    test.C = true;
    test.Z = false;
    test.V = false;
    test.N = false;

    TestImmediate(0xE9, test);
}

TEST_F(ArithmeticTests, SBCOneFromZero)
{
    TestVariables test;
    test.A = 0;
    test.M = 1;
    test.result = (uint8_t) -1;
    test.carry = true;
    test.C = false;
    test.Z = false;
    test.V = false;
    test.N = true;

    TestImmediate(0xE9, test);
}

TEST_F(ArithmeticTests, SBCCarry)
{
    TestVariables test;
    test.A = 0;
    test.M = 1;
    test.result = (uint8_t) -2;
    test.carry = false;
    test.C = false;
    test.Z = false;
    test.V = false;
    test.N = true;

    TestImmediate(0xE9, test);
}

TEST_F(ArithmeticTests, SBCOverflow)
{
    TestVariables test;
    test.A = (uint8_t) -128;
    test.M = 1;
    test.result = 127;
    test.carry = true;
    test.C = true;
    test.Z = false;
    test.V = true;
    test.N = false;

    TestImmediate(0xE9, test);
}

TEST_F(ArithmeticTests, SBCOverflow2)
{
    TestVariables test;
    test.A = 127;
    test.M = (uint8_t) -1;
    test.result = 128;
    test.carry = true;
    test.C = false;
    test.Z = false;
    test.V = true;
    test.N = true;

    TestImmediate(0xE9, test);
}