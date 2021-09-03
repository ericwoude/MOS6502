#ifndef MOS6502_H
#define MOS6502_H

#include <stdint.h>
#include <assert.h>
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
    uint8_t C : 1;
    uint8_t Z : 1;
    uint8_t I : 1;
    uint8_t D : 1;
    uint8_t B : 1;
    uint8_t V : 1;
    uint8_t N : 1;
 
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
        JPS_A = 0x20;

    private:
    // Addressing modes
    uint8_t ImmediateAddressing(uint32_t& machineCycles, Mem& memory);
    uint8_t ZPAddressing(uint32_t& machineCycles, Mem& memory);
    uint8_t ZPXAddressing(uint32_t& machineCycles, Mem& memory);
    uint8_t ABSAddressing(uint32_t& machineCycles, Mem& memory);
    uint8_t ABSXAddressing(uint32_t& machineCycles, Mem& memory);
    uint8_t ABSYAddressing(uint32_t& machineCycles, Mem& memory);
    uint8_t INDXAddressing(uint32_t& machineCycles, Mem& memory);
    uint8_t INDYAddressing(uint32_t& machineCycles, Mem& memory);

    // Fetch a single byte from memory offsetted by the PC.
    uint8_t FetchByte(uint32_t& machineCycles, Mem& memory);
    uint16_t FetchWord(uint32_t& machineCycles, Mem& memory);

    // Like FetchByte, except it fetches using an address and it does not
    // increment the program counter.
    uint8_t ReadByte(uint32_t& machineCycles, uint16_t address, Mem& memory);
    uint16_t ReadWord(uint32_t& machineCycles, uint16_t address, Mem& memory);
};

#endif // MOS6502_H