#ifndef MOS6502_H
#define MOS6502_H

#include <stdint.h>
#include <assert.h>
#include <bitset>
#include <array>
#include <map>
#include <iostream>

class Mem
{
    public:
    void Initialize();
    void WriteWord(uint16_t value, uint32_t address, uint32_t& machineCycles);
 
    // Enables reading and writing to memory using the [] operator.
    uint8_t operator[](uint32_t address) const;
    uint8_t& operator[](uint32_t address);

    private:
    static const size_t maxSize = 64 * 1024;
    std::array<uint8_t, maxSize> data;
};




class CPU
{
    public:
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
            uint8_t V : 1;
            uint8_t N : 1;
            uint8_t _ : 1; // Unused last bit
        };
    };
 
    void Reset(Mem& memory);
    void Execute(uint32_t machineCycles, Mem& memory);
 
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
    // Addressing modes
    uint8_t ImmediateAddressing(uint32_t& machineCycles, Mem& memory);
    uint8_t ZPAddressing(uint32_t& machineCycles, Mem& memory);
    uint8_t ZPXAddressing(uint32_t& machineCycles, Mem& memory);
    uint8_t ZPYAddressing(uint32_t& machineCycles, Mem& memory);
    uint16_t ABSAddressing(uint32_t& machineCycles, Mem& memory);
    uint16_t ABSXAddressing(uint32_t& machineCycles, Mem& memory);
    uint16_t ABSXAddressing5(uint32_t& machineCycles, Mem& memory);
    uint16_t ABSYAddressing(uint32_t& machineCycles, Mem& memory);
    uint16_t ABSYAddressing5(uint32_t& machineCycles, Mem& memory);
    uint16_t INDXAddressing(uint32_t& machineCycles, Mem& memory);
    uint16_t INDYAddressing(uint32_t& machineCycles, Mem& memory);
    uint16_t INDYAddressing6(uint32_t& machineCycles, Mem& memory);

    // Sets the Z, N flag for the LDA, LDX and LDY instructions.
    void LDSetFlags(uint8_t reg);

    // Fetch a single byte from memory offsetted by the PC.
    uint8_t FetchByte(uint32_t& machineCycles, Mem& memory);
    uint16_t FetchWord(uint32_t& machineCycles, Mem& memory);

    // Like FetchByte, except it fetches using an address and it does not
    // increment the program counter.
    uint8_t ReadByte(uint32_t& machineCycles, uint16_t address, Mem& memory);
    void StoreByte(uint32_t& machineCycles, uint16_t address, uint8_t value, Mem& memory);

    uint16_t ReadWord(uint32_t& machineCycles, uint16_t address, Mem& memory);
    // void StoreWord(uint32_t& machineCycles, uint16_t address, uint16_t value, Mem& memory);

    // Stack operations
    void PushToStack(uint32_t& machineCycles, uint8_t value, Mem& memory);
    uint8_t PullFromStack(uint32_t& machineCycles, Mem& memory);
};

#endif // MOS6502_H