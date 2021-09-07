#include <gtest/gtest.h>

#include "cpu.h"

#include <functional>

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

    void OpImmediate(uint8_t opcode, const std::function<int( int, int )>& f)
    {
        cpu.A = 0b10101010;

        mem[0xFFFC] = opcode;
		mem[0xFFFD] = 0b00001000;

        const uint32_t cycles = 2;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

		EXPECT_EQ( cpu.A, f(0b10101010, 0b00001000) );
		EXPECT_EQ( used_cycles, cycles );
		EXPECT_FALSE( cpu.Z );
    }

    void OpZeroPage(uint8_t opcode, const std::function<int( int, int )>& f)
    {
        cpu.A = 0b10101010;

        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x22;
        mem[0x0022] = 0b00001000;

        const uint32_t cycles = 3;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(cpu.A, f(0b10101010, 0b00001000));
        EXPECT_EQ(cycles, used_cycles);
        EXPECT_FALSE(cpu.Z);
        EXPECT_FALSE(cpu.N);
    }

    void OpZeroPageX(uint8_t opcode, const std::function<int( int, int )>& f)
    {
        cpu.X = 0x04;
        cpu.A = 0b10101010;

        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x05;
        mem[0x0009] = 0b00001000;

        const uint32_t cycles = 4;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(cpu.A, f(0b10101010, 0b00001000));
        EXPECT_EQ(cycles, used_cycles);
        EXPECT_FALSE(cpu.Z);
        EXPECT_FALSE(cpu.N);
    }

    void OpAbsolute(uint8_t opcode, const std::function<int( int, int )>& f)
    {
        cpu.X = 0x04;
        cpu.A = 0b10101010;

        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x05;
        mem[0xFFFE] = 0x05;
        mem[0x0505] = 0b00001000;

        const uint32_t cycles = 4;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(cpu.A, f(0b10101010, 0b00001000));
        EXPECT_EQ(cycles, used_cycles);
        EXPECT_FALSE(cpu.Z);
        EXPECT_FALSE(cpu.N);
    }

    void OpAbsoluteX(uint8_t opcode, const std::function<int(int, int)>& f)
    {
        cpu.X = 0x04;
        cpu.A = 0b10101010;

        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x05;
        mem[0xFFFE] = 0x05;
        mem[0x0509] = 0b00001000;

        const uint32_t cycles = 4;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(cpu.A, f(0b10101010, 0b00001000));
        EXPECT_EQ(cycles, used_cycles);
        EXPECT_FALSE(cpu.Z);
        EXPECT_FALSE(cpu.N);
    }

    void OpAbsoluteXPageCrossed(uint8_t opcode, const std::function<int(int, int)>& f)
    {
        cpu.X = 0xFF;
        cpu.A = 0b10101010;

        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x01;
        mem[0xFFFE] = 0x05;
        mem[0x0600] = 0b00001000; // Crossed page boundary

        const uint32_t cycles = 5;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(cpu.A, f(0b10101010, 0b00001000));
        EXPECT_EQ(cycles, used_cycles);
        EXPECT_FALSE(cpu.Z);
        EXPECT_FALSE(cpu.N);
    }

    void OpAbsoluteY(uint8_t opcode, const std::function<int(int, int)>& f)
    {
        cpu.Y = 0x04;
        cpu.A = 0b10101010;

        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x05;
        mem[0xFFFE] = 0x05;
        mem[0x0509] = 0b00001000;

        const uint32_t cycles = 4;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(cpu.A, f(0b10101010, 0b00001000));
        EXPECT_EQ(cycles, used_cycles);
        EXPECT_FALSE(cpu.Z);
        EXPECT_FALSE(cpu.N);
    }

    void OpAbsoluteYPageCrossed(uint8_t opcode, const std::function<int(int, int)>& f)
    {
        cpu.Y = 0xFF;
        cpu.A = 0b10101010;

        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x01;
        mem[0xFFFE] = 0x05;
        mem[0x0600] = 0b00001000; // Crossed page boundary

        const uint32_t cycles = 5;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(cpu.A, f(0b10101010, 0b00001000));
        EXPECT_EQ(cycles, used_cycles);
        EXPECT_FALSE(cpu.Z);
        EXPECT_FALSE(cpu.N);
    }

    void OpIndexedIndirect(uint8_t opcode, const std::function<int(int, int)>& f)
    {
        cpu.X = 0x02;
        cpu.A = 0b10101010;

        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x05;
        mem[0x0007] = 0x0A;
        mem[0x0008] = 0x0A;
        mem[0x0A0A] = 0b00001000;

        const uint32_t cycles = 6;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(cpu.A, f(0b10101010, 0b00001000));
        EXPECT_EQ(cycles, used_cycles);
        EXPECT_FALSE(cpu.Z);
        EXPECT_FALSE(cpu.N);
    }

    void OpIndexedIndirectPageCrossed(uint8_t opcode, const std::function<int(int, int)>& f)
    {
        cpu.X = 0xFF;
        cpu.A = 0b10101010;

        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x01;
        mem[0x0000] = 0x0A; // (0x01 + 0xFF) & 0xFF = 0x00
        mem[0x0001] = 0x0A;
        mem[0x0A0A] = 0b00001000;

        const uint32_t cycles = 6;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(cpu.A, f(0b10101010, 0b00001000));
        EXPECT_EQ(cycles, used_cycles);
        EXPECT_FALSE(cpu.Z);
        EXPECT_FALSE(cpu.N);
    }

    void OpIndirectIndexed(uint8_t opcode, const std::function<int(int, int)>& f)
    {
        cpu.Y = 0x02;
        cpu.A = 0b10101010;

        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x02;
        mem[0x0002] = 0x00;
        mem[0x0003] = 0x80;
        mem[0x8002] = 0b00001000;

        const uint32_t cycles = 5;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(cpu.A, f(0b10101010, 0b00001000));
        EXPECT_EQ(cycles, used_cycles);
        EXPECT_FALSE(cpu.Z);
        EXPECT_FALSE(cpu.N);
    }

    void OpIndirectIndexedPageCrossed(uint8_t opcode, const std::function<int(int, int)>& f)
    {
        cpu.Y = 0xFF;
        cpu.A = 0b10101010;

        mem[0xFFFC] = opcode;
        mem[0xFFFD] = 0x02;
        mem[0x0002] = 0x01;
        mem[0x0003] = 0x05;
        mem[0x0600] = 0b00001000; // Crossed page boundary

        const uint32_t cycles = 6;
        uint32_t used_cycles = cpu.Execute(cycles, mem);

        EXPECT_EQ(cpu.A, f(0b10101010, 0b00001000));
        EXPECT_EQ(cycles, used_cycles);
        EXPECT_FALSE(cpu.Z);
        EXPECT_FALSE(cpu.N);
    }
};

// TESTS FOR AND

TEST_F(LogicalTests, ANDImmediate)
{
    OpImmediate(0x29, [](int x, int y)
    {
        return x & y;
    });
}

TEST_F(LogicalTests, ANDZeroPage)
{
    OpZeroPage(0x25, [](int x, int y)
    {
        return x & y;
    });
}

TEST_F(LogicalTests, ANDZeroPageX)
{
    OpZeroPageX(0x35, [](int x, int y)
    {
        return x & y;
    });
}

TEST_F(LogicalTests, ANDAbsolute)
{
    OpAbsolute(0x2D, [](int x, int y)
    {
        return x & y;
    });
}

TEST_F(LogicalTests, ANDAbsoluteX)
{
    OpAbsoluteX(0x3D, [](int x, int y)
    {
        return x & y;
    });
}

TEST_F(LogicalTests, ANDAbsoluteXPageCrossed)
{
    
    OpAbsoluteXPageCrossed(0x3D, [](int x, int y)
    {
        return x & y;
    });
}

TEST_F(LogicalTests, ANDAbsoluteY)
{
    OpAbsoluteY(0x39, [](int x, int y)
    {
        return x & y;
    });
}

TEST_F(LogicalTests, ANDAbsoluteYPageCrossed)
{
    OpAbsoluteYPageCrossed(0x39, [](int x, int y)
    {
        return x & y;
    });
}

TEST_F(LogicalTests, ANDIndexedIndirect)
{
    OpIndexedIndirect(0x21, [](int x, int y)
    {
        return x & y;
    });
}

TEST_F(LogicalTests, ANDIndexedIndirectPageCrossed)
{
    OpIndexedIndirectPageCrossed(0x21, [](int x, int y)
    {
        return x & y;
    });
}

TEST_F(LogicalTests, ANDIndirectIndexed)
{
    OpIndirectIndexed(0x31, [](int x, int y)
    {
        return x & y;
    });
}

TEST_F(LogicalTests, ANDIndirectIndexedPageCrossed)
{
    OpIndirectIndexedPageCrossed(0x31, [](int x, int y)
    {
        return x & y;
    });
}

// TESTS FOR EOR

TEST_F(LogicalTests, EORImmediate)
{
    OpImmediate(0x49, [](int x, int y)
    {
        return x ^ y;
    });
}

TEST_F(LogicalTests, EORZeroPage)
{
    OpZeroPage(0x45, [](int x, int y)
    {
        return x ^ y;
    });
}

TEST_F(LogicalTests, EORZeroPageX)
{
    OpZeroPageX(0x55, [](int x, int y)
    {
        return x ^ y;
    });
}

TEST_F(LogicalTests, EORAbsolute)
{
    OpAbsolute(0x4D, [](int x, int y)
    {
        return x ^ y;
    });
}

TEST_F(LogicalTests, EORAbsoluteX)
{
    OpAbsoluteX(0x5D, [](int x, int y)
    {
        return x ^ y;
    });
}

TEST_F(LogicalTests, EORAbsoluteXPageCrossed)
{
    
    OpAbsoluteXPageCrossed(0x5D, [](int x, int y)
    {
        return x ^ y;
    });
}

TEST_F(LogicalTests, EORAbsoluteY)
{
    OpAbsoluteY(0x59, [](int x, int y)
    {
        return x ^ y;
    });
}

TEST_F(LogicalTests, EORAbsoluteYPageCrossed)
{
    OpAbsoluteYPageCrossed(0x59, [](int x, int y)
    {
        return x ^ y;
    });
}

TEST_F(LogicalTests, EORIndexedIndirect)
{
    OpIndexedIndirect(0x41, [](int x, int y)
    {
        return x ^ y;
    });
}

TEST_F(LogicalTests, EORIndexedIndirectPageCrossed)
{
    OpIndexedIndirectPageCrossed(0x41, [](int x, int y)
    {
        return x ^ y;
    });
}

TEST_F(LogicalTests, EORIndirectIndexed)
{
    OpIndirectIndexed(0x51, [](int x, int y)
    {
        return x ^ y;
    });
}

TEST_F(LogicalTests, EORIndirectIndexedPageCrossed)
{
    OpIndirectIndexedPageCrossed(0x51, [](int x, int y)
    {
        return x ^ y;
    });
}

// TESTS FOR ORA

TEST_F(LogicalTests, ORAImmediate)
{
    OpImmediate(0x09, [](int x, int y)
    {
        return x | y;
    });
}

TEST_F(LogicalTests, ORAZeroPage)
{
    OpZeroPage(0x05, [](int x, int y)
    {
        return x | y;
    });
}

TEST_F(LogicalTests, ORAZeroPageX)
{
    OpZeroPageX(0x15, [](int x, int y)
    {
        return x | y;
    });
}

TEST_F(LogicalTests, ORAAbsolute)
{
    OpAbsolute(0x0D, [](int x, int y)
    {
        return x | y;
    });
}

TEST_F(LogicalTests, ORAAbsoluteX)
{
    OpAbsoluteX(0x1D, [](int x, int y)
    {
        return x | y;
    });
}

TEST_F(LogicalTests, ORAAbsoluteXPageCrossed)
{
    
    OpAbsoluteXPageCrossed(0x1D, [](int x, int y)
    {
        return x | y;
    });
}

TEST_F(LogicalTests, ORAAbsoluteY)
{
    OpAbsoluteY(0x19, [](int x, int y)
    {
        return x | y;
    });
}

TEST_F(LogicalTests, ORAAbsoluteYPageCrossed)
{
    OpAbsoluteYPageCrossed(0x19, [](int x, int y)
    {
        return x | y;
    });
}

TEST_F(LogicalTests, ORAIndexedIndirect)
{
    OpIndexedIndirect(0x01, [](int x, int y)
    {
        return x | y;
    });
}

TEST_F(LogicalTests, ORAIndexedIndirectPageCrossed)
{
    OpIndexedIndirectPageCrossed(0x01, [](int x, int y)
    {
        return x | y;
    });
}

TEST_F(LogicalTests, ORAIndirectIndexed)
{
    OpIndirectIndexed(0x11, [](int x, int y)
    {
        return x | y;
    });
}

TEST_F(LogicalTests, ORAIndirectIndexedPageCrossed)
{
    OpIndirectIndexedPageCrossed(0x11, [](int x, int y)
    {
        return x | y;
    });
}