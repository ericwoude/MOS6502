#include "MOS6502.h"

/**************
 * MEM
 **************/

void Mem::Initialize()
{
    data.fill(0);
}

// Read a single byte from memory.
uint8_t Mem::operator[](uint32_t address) const
{
    assert (address < maxSize);
    return data[address];
}

// Write a single byte to memory.
uint8_t& Mem::operator[](uint32_t address)
{
    assert (address < maxSize);
    return data[address];
}

void Mem::WriteWord(uint16_t value, uint32_t address, uint32_t& machineCycles)
{
    data[address] = value & 0xFF;
    data[address + 1] = value >> 8;

    machineCycles -= 2;
}

/**************
 * CPU
 **************/

void CPU::Reset(Mem& memory)
{
    PC = 0xFFFC;
    SP = 0x0100;

    // Clear processor status flags
    C = 0; Z = 0; I = 0; D = 0; B = 0; V = 0; N = 0;

    // Reset general-purpose registers
    A = 0; X = 0; Y = 0;

    // Fill memory with 0s.
    memory.Initialize();
}

// Fetch a single byte from memory offsetted by the PC.
uint8_t CPU::FetchByte(uint32_t& machineCycles, Mem& memory)
{
    uint8_t b = memory[PC];
    PC++;
    machineCycles--;

    return b;
}

uint16_t CPU::FetchWord(uint32_t& machineCycles, Mem& memory)
{
    uint16_t w = memory[PC];
    w |= (memory[PC + 1] << 8);

    PC += 2;
    machineCycles -=2;

    return w;
}

// Like FetchByte, except it fetches using an address and it does not
// increment the program counter.
uint8_t CPU::ReadByte(uint32_t& machineCycles, uint16_t address, Mem& memory)
{
    uint8_t b = memory[address];
    machineCycles--;

    return b;
}

uint16_t CPU::ReadWord(uint32_t& machineCycles, uint16_t address, Mem& memory)
{
    uint8_t l = ReadByte(machineCycles, address, memory);
    uint8_t h = ReadByte(machineCycles, address + 1, memory);

    return (h << 8) | l;
}

// Addressing modes
uint8_t CPU::ImmediateAddressing(uint32_t& machineCycles, Mem& memory)
{
    return FetchByte(machineCycles, memory);
}

uint8_t CPU::ZPAddressing(uint32_t& machineCycles, Mem& memory)
{
    uint8_t zeroPageAddress = FetchByte(machineCycles, memory);
    return ReadByte(machineCycles, zeroPageAddress, memory);
}

uint8_t CPU::ZPXAddressing(uint32_t& machineCycles, Mem& memory)
{
    uint8_t zeroPageAddress = FetchByte(machineCycles, memory);

    // If it exceeds the zero page, wrap around.
    uint8_t zeroPageAddressXWrap = (zeroPageAddress + X) & 0xFF;
    machineCycles--;

    return ReadByte(machineCycles, zeroPageAddressXWrap, memory);
}

uint8_t CPU::ABSAddressing(uint32_t& machineCycles, Mem& memory)
{
    uint16_t absAddress = FetchWord(machineCycles, memory);
    return ReadByte(machineCycles, absAddress, memory);
}

uint8_t CPU::ABSXAddressing(uint32_t& machineCycles, Mem& memory)
{
    uint16_t absAddress = FetchWord(machineCycles, memory);
    uint16_t sum = absAddress + X;

    // Newly computed address crossed the page boundary.
    if (sum - absAddress >= 0xFF)
        machineCycles--;

    return ReadByte(machineCycles, sum, memory);
}

uint8_t CPU::ABSYAddressing(uint32_t& machineCycles, Mem& memory)
{
    uint16_t absAddress = FetchWord(machineCycles, memory);
    uint16_t sum = absAddress + Y;

    // Newly computed address crossed the page boundary.
    if (sum - absAddress >= 0xFF)
        machineCycles--;
    
    return ReadByte(machineCycles, sum, memory);
}

uint8_t CPU::INDXAddressing(uint32_t& machineCycles, Mem& memory)
{
    uint16_t address = FetchByte(machineCycles, memory);
    uint16_t target = ReadWord(machineCycles, address + X, memory);
    machineCycles--;

    return ReadByte(machineCycles, target, memory);
}

uint8_t CPU::INDYAddressing(uint32_t& machineCycles, Mem& memory)
{
    uint8_t zeroPageAddress = FetchByte(machineCycles, memory);
    uint16_t effectiveAddress = ReadWord(machineCycles, zeroPageAddress, memory);
    uint16_t effectiveAddressY = effectiveAddress + Y;

    if (effectiveAddressY - effectiveAddress >= 0xFF)
            machineCycles--;

    return ReadByte(machineCycles, effectiveAddressY, memory);
}

// Instruction specific functions
void CPU::LDSetFlags()
{
    Z = (A == 0);
    N = (A & 0b1000000) > 0;
}

void CPU::Execute(uint32_t machineCycles, Mem& memory)
{
    while (machineCycles > 0)
    {
        uint8_t instruction = FetchByte(machineCycles, memory);

        switch (instruction)
        {
            ////////////////////////////////////
            // LDA
            ////////////////////////////////////

            case LDA_IM:
            {
                A = ImmediateAddressing(machineCycles, memory);
                LDSetFlags();
            } break;

            // Fetches zero page address from instruction,
            // then uses zero page address to read a byte from
            // the zero page.
            case LDA_ZP:
            {
                A = ZPAddressing(machineCycles, memory);
                LDSetFlags();
            } break;

            // Like LDA_ZP, except it adds the contents of register X
            // to the zero page address. If zeroPageAddress + X exceeds
            // the zero page size (FF), then it will wrap around.
            case LDA_ZPX:
            {
                A = ZPXAddressing(machineCycles, memory);
                LDSetFlags();
            } break;

            // Fetches an absolute address (word) from the PC by
            // concatenating two bytes. Then reads the byte at the
            // absolute address.
            case LDA_ABS:
            {
                A = ABSAddressing(machineCycles, memory);
                LDSetFlags();;
            } break;

            // Like LDA_ABS, except adds the contents of register X
            // to the absolute address.
            case LDA_ABSX:
            {
                A = ABSXAddressing(machineCycles, memory);
                LDSetFlags();      
            } break;

            // Like LDA_ABSX, except it uses the Y register instead of
            // the X register.
            case LDA_ABSY:
            {
                A = ABSYAddressing(machineCycles, memory);
                LDSetFlags();  
            } break;

            // Fetches an address, adds the contents of X to it, and
            // uses the sum to read the effective address from the zero page.
            // Finally, it reads a byte into A from memory ofsetted by the
            // effective address.
            case LDA_INDX:
            {
                A = INDXAddressing(machineCycles, memory);
                LDSetFlags();
            } break;

            // Fetches address from memory. Fetches a new target address
            // from memory using the old address. Reads the memory at
            // target + general register Y. Consumes an extra machine
            // cycle if the memory is read out of page bound.
            case LDA_INDY:
            {
                A = INDYAddressing(machineCycles, memory);
                LDSetFlags();
            } break;

            // case INS_JPS_A:
            // {
            //     uint16_t address = FetchWord(machineCycles, memory);
            //     memory.WriteWord(PC - 1, SP++, machineCycles);

            //     PC = address;
            //     machineCycles--;
            // } break;

            default:
            {
                std::cout << "Unknown instruction: 0x"
                            << std::hex << instruction + 0 << std::endl;
            } break;
        }
    }
}