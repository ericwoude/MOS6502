#ifndef CPU_H
#define CPU_H

#include "mem.h"

#include <array>
#include <cstdint>
#include <cassert>
#include <iostream>

class CPU
{
    public:
    CPU();
    void Reset(Mem& memory);
    uint32_t Execute(uint32_t machine_cycles, Mem& memory);

    // Program counter, stack pointer and general-purpose registers A, X and Y.
    uint16_t PC;
    uint8_t SP;
    uint8_t A, X, Y;

    // Processor status flags
    union
    {
        uint8_t PS;
        struct
        {
            uint8_t C : 1;
            uint8_t Z : 1;
            uint8_t I : 1;
            uint8_t D : 1;
            uint8_t B : 1;
            uint8_t _ : 1; // Unused last bit
            uint8_t V : 1;
            uint8_t N : 1;
        };
    };
 
    // Opcodes
    static constexpr uint8_t
        LDA_IM = 0xA9,
        LDA_ZP = 0xA5,
        LDA_ZPX = 0xB5,
        LDA_ABS = 0x6D,
        LDA_ABSX = 0xBD,
        LDA_ABSY = 0xB9,
        LDA_INDX = 0xA1,
        LDA_INDY = 0xB1,

        LDX_IM = 0xA2,
        LDX_ZP = 0xA6,
        LDX_ZPY = 0xB6,
        LDX_ABS = 0xAE,
        LDX_ABSY = 0xBE,

        LDY_IM = 0xA0,
        LDY_ZP = 0xA4,
        LDY_ZPX = 0xB4,
        LDY_ABS = 0xAC,
        LDY_ABSX = 0xBC,

        STA_ZP = 0x85,
        STA_ZPX = 0x95,
        STA_ABS = 0x8D,
        STA_ABSX = 0x9D,
        STA_ABSY = 0x99,
        STA_INDX = 0x81,
        STA_INDY = 0x91,

        STX_ZP = 0x86,
        STX_ZPY = 0x96,
        STX_ABS = 0x8E,

        STY_ZP = 0x84,
        STY_ZPX = 0x94,
        STY_ABS = 0x8C,

        TAX = 0xAA,
        TAY = 0xA8,
        TXA = 0x8A,
        TYA = 0x98,

        TSX = 0xBA,
        TXS = 0x9A,
        PHA = 0x48,
        PHP = 0x08,
        PLA = 0x68,
        PLP = 0x28,

        JPS_A = 0x20;

    private:
    using AddressExecution = uint16_t (CPU::*)(uint32_t&, Mem&);
    using OperationExecution = void (CPU::*)(uint32_t&, uint16_t, Mem&);

    struct Instruction {
        AddressExecution addr;
        OperationExecution op;
    };

    std::array<Instruction, 256> dispatch_table;
    void exec_instruction(Instruction instruction, uint32_t& machine_cycles, Mem& memory);

    // Addressing mode functions
    uint16_t AddrOpcode(uint32_t&, Mem& memory); // Used for debugging illegal opcodes
    uint16_t AddrImplied(uint32_t&, Mem&); // Does not do anything
    uint16_t AddrImmediate(uint32_t&, Mem&);
    uint16_t AddrZeroPage(uint32_t& machine_cycles, Mem& memory);
    uint16_t AddrZeroPageX(uint32_t& machine_cycles, Mem& memory);
    uint16_t AddrZeroPageY(uint32_t& machine_cycles, Mem& memory);
    uint16_t AddrAbsolute(uint32_t& machine_cycles, Mem& memory);
    uint16_t AddrAbsoluteX(uint32_t& machine_cycles, Mem& memory);
    uint16_t AddrAbsoluteX5(uint32_t& machine_cycles, Mem& memory);
    uint16_t AddrAbsoluteY(uint32_t& machine_cycles, Mem& memory);
    uint16_t AddrAbsoluteY5(uint32_t& machine_cycles, Mem& memory);
    uint16_t AddrIndexedIndirect(uint32_t& machine_cycles, Mem& memory);
    uint16_t AddrIndirectIndexed(uint32_t& machine_cycles, Mem& memory);
    uint16_t AddrIndirectIndexed6(uint32_t& machine_cycles, Mem& memory);

    // Sets the Z, N flag for the LDA, LDX and LDY instructions.
    void LDSetFlags(uint8_t reg);

    // Fetch a single byte from memory offsetted by the PC.
    uint8_t FetchByte(uint32_t& machine_cycles, Mem& memory);
    uint16_t FetchWord(uint32_t& machine_cycles, Mem& memory);

    // Like FetchByte, except it fetches using an address and it does not
    // increment the program counter.
    uint8_t ReadByte(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void StoreByte(uint32_t& machine_cycles, uint16_t address, uint8_t value, Mem& memory);

    uint16_t ReadWord(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    // void StoreWord(uint32_t& machine_cycles, uint16_t address, uint16_t value, Mem& memory);

    // Stack operations
    void PushToStack(uint32_t& machine_cycles, uint8_t value, Mem& memory);
    uint8_t PullFromStack(uint32_t& machine_cycles, Mem& memory);

    // Operation functions
    // LOAD & STORE
    void OpLDA(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpLDX(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpLDY(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpSTA(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpSTX(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpSTY(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    
    // REGISTER TRANSFERS
    void OpTAX(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpTAY(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpTXA(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpTYA(uint32_t& machine_cycles, uint16_t address, Mem& memory);

    // STACK OPERATIONS
    void OpTSX(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpTXS(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpPHA(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpPHP(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpPLA(uint32_t& machine_cycles, uint16_t address, Mem& memory);
    void OpPLP(uint32_t& machine_cycles, uint16_t address, Mem& memory);

    void OpIllegal(uint32_t& machine_cycles, uint16_t address, Mem& memory);
};

#endif // CPU_H