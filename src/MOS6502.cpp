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
    assert (address <= maxSize);
    return data[address];
}

// Write a single byte to memory.
uint8_t& Mem::operator[](uint32_t address)
{
    assert (address <= maxSize);
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

void CPU::StoreByte(uint32_t& machineCycles, uint16_t address, uint8_t value, Mem& memory)
{
    memory[address] = value;
    machineCycles--;
}

// void CPU::StoreWord(uint32_t& machineCycles, uint16_t address, uint16_t value, Mem& memory)
// {
//     memory[address]     = (uint8_t) value;
//     memory[address + 1] = (value << 8);

//     machineCycles -= 2;
// }

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
    return FetchByte(machineCycles, memory);
}

uint8_t CPU::ZPXAddressing(uint32_t& machineCycles, Mem& memory)
{
    uint8_t zeroPageAddress = FetchByte(machineCycles, memory);

    // If it exceeds the zero page, wrap around.
    uint8_t zeroPageAddressXWrap = (zeroPageAddress + X) & 0xFF;
    machineCycles--;

    return zeroPageAddressXWrap;
}

uint8_t CPU::ZPYAddressing(uint32_t& machineCycles, Mem& memory)
{
    uint8_t zeroPageAddress = FetchByte(machineCycles, memory);

    // If it exceeds the zero page, wrap around.
    uint8_t zeroPageAddressYWrap = (zeroPageAddress + Y) & 0xFF;
    machineCycles--;

    return zeroPageAddressYWrap;
}

uint16_t CPU::ABSAddressing(uint32_t& machineCycles, Mem& memory)
{
    return FetchWord(machineCycles, memory);
}

uint16_t CPU::ABSXAddressing(uint32_t& machineCycles, Mem& memory)
{
    uint16_t absAddress = FetchWord(machineCycles, memory);
    uint16_t sum = absAddress + X;

    // Newly computed address crossed the page boundary.
    if (sum - absAddress >= 0xFF)
        machineCycles--;

    return sum;
}

uint16_t CPU::ABSXAddressing5(uint32_t& machineCycles, Mem& memory)
{
    uint16_t absAddress = FetchWord(machineCycles, memory);
    uint16_t sum = absAddress + X;
    machineCycles--;

    return sum;
}

uint16_t CPU::ABSYAddressing(uint32_t& machineCycles, Mem& memory)
{
    uint16_t absAddress = FetchWord(machineCycles, memory);
    uint16_t sum = absAddress + Y;

    // Newly computed address crossed the page boundary.
    if (sum - absAddress >= 0xFF)
        machineCycles--;
    
    return sum;
}

uint16_t CPU::ABSYAddressing5(uint32_t& machineCycles, Mem& memory)
{
    uint16_t absAddress = FetchWord(machineCycles, memory);
    uint16_t sum = absAddress + Y;
    machineCycles--;

    return sum;
}

uint16_t CPU::INDXAddressing(uint32_t& machineCycles, Mem& memory)
{
    uint16_t address = FetchByte(machineCycles, memory);
    uint16_t target = ReadWord(machineCycles, (address + X) & 0xFF, memory);
    machineCycles--;

    return target;
}

uint16_t CPU::INDYAddressing(uint32_t& machineCycles, Mem& memory)
{
    uint8_t zeroPageAddress = FetchByte(machineCycles, memory);
    uint16_t target = ReadWord(machineCycles, zeroPageAddress, memory);
    uint16_t targetY = target + Y;

    if (targetY - target >= 0xFF)
            machineCycles--;

    return targetY;
}

uint16_t CPU::INDYAddressing6(uint32_t& machineCycles, Mem& memory)
{
    uint8_t zeroPageAddress = FetchByte(machineCycles, memory);
    uint16_t target = ReadWord(machineCycles, zeroPageAddress, memory);
    uint16_t targetY = target + Y;
    machineCycles--;

    return targetY;
}


// Instruction specific functions
void CPU::LDSetFlags(uint8_t reg)
{
    Z = (reg == 0);
    N = (reg & 0b1000000) > 0;
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
                LDSetFlags(A);
            } break;

            // Fetches zero page address from instruction,
            // then uses zero page address to read a byte from
            // the zero page.
            case LDA_ZP:
            {
                uint8_t zeroPageAddress = ZPAddressing(machineCycles, memory);
                A = ReadByte(machineCycles, zeroPageAddress, memory);
                LDSetFlags(A);
            } break;

            // Like LDA_ZP, except it adds the contents of register X
            // to the zero page address. If zeroPageAddress + X exceeds
            // the zero page size (FF), then it will wrap around.
            case LDA_ZPX:
            {
                uint8_t zeroPageAddress = ZPXAddressing(machineCycles, memory);
                A = ReadByte(machineCycles, zeroPageAddress, memory);
                LDSetFlags(A);
            } break;

            // Fetches an absolute address (word) from the PC by
            // concatenating two bytes. Then reads the byte at the
            // absolute address.
            case LDA_ABS:
            {
                uint16_t absAddress = ABSAddressing(machineCycles, memory);
                A = ReadByte(machineCycles, absAddress, memory);
                LDSetFlags(A);
            } break;

            // Like LDA_ABS, except adds the contents of register X
            // to the absolute address.
            case LDA_ABSX:
            {
                uint16_t absAddress = ABSXAddressing(machineCycles, memory);
                A = ReadByte(machineCycles, absAddress, memory);
                LDSetFlags(A);
            } break;

            // Like LDA_ABSX, except it uses the Y register instead of
            // the X register.
            case LDA_ABSY:
            {
                uint16_t absAddress = ABSYAddressing(machineCycles, memory);
                A = ReadByte(machineCycles, absAddress, memory);
                LDSetFlags(A);
            } break;

            // Fetches an address, adds the contents of X to it, and
            // uses the sum to read the effective address from the zero page.
            // Finally, it reads a byte into A from memory ofsetted by the
            // effective address.
            case LDA_INDX:
            {
                uint16_t target = INDXAddressing(machineCycles, memory);
                A = ReadByte(machineCycles, target, memory);
                LDSetFlags(A);
            } break;

            // Fetches address from memory. Fetches a new target address
            // from memory using the old address. Reads the memory at
            // target + general register Y. Consumes an extra machine
            // cycle if the memory is read out of page bound.
            case LDA_INDY:
            {
                uint16_t target = INDYAddressing(machineCycles, memory);
                A = ReadByte(machineCycles, target, memory);
                LDSetFlags(A);
            } break;

            ////////////////////////////////////
            // LDX
            ////////////////////////////////////

            case LDX_IM:
            {
                X = ImmediateAddressing(machineCycles, memory);
                LDSetFlags(X);
            } break;

            case LDX_ZP:
            {
                uint8_t zeroPageAddress = ZPAddressing(machineCycles, memory);
                X = ReadByte(machineCycles, zeroPageAddress, memory);
                LDSetFlags(X);
            } break;

            case LDX_ZPY:
            {
                uint8_t zeroPageAddress = ZPYAddressing(machineCycles, memory);
                X = ReadByte(machineCycles, zeroPageAddress, memory);
                LDSetFlags(X);
            } break;

            case LDX_ABS:
            {
                uint16_t absAddress = ABSAddressing(machineCycles, memory);
                X = ReadByte(machineCycles, absAddress, memory);
                LDSetFlags(X);
            } break;

            case LDX_ABSY:
            {
                uint16_t absAddress = ABSYAddressing(machineCycles, memory);
                X = ReadByte(machineCycles, absAddress, memory);
                LDSetFlags(X);
            } break;

            ////////////////////////////////////
            // LDY
            ////////////////////////////////////

            case LDY_IM:
            {
                Y = ImmediateAddressing(machineCycles, memory);
                LDSetFlags(Y);
            } break;

            case LDY_ZP:
            {
                uint8_t zeroPageAddress = ZPAddressing(machineCycles, memory);
                Y = ReadByte(machineCycles, zeroPageAddress, memory);
                LDSetFlags(Y);
            } break;

            case LDY_ZPX:
            {
                uint8_t zeroPageAddress = ZPXAddressing(machineCycles, memory);
                Y = ReadByte(machineCycles, zeroPageAddress, memory);
                LDSetFlags(Y);
            } break;

            case LDY_ABS:
            {
                uint16_t absAddress = ABSAddressing(machineCycles, memory);
                Y = ReadByte(machineCycles, absAddress, memory);
                LDSetFlags(Y);
            } break;

            case LDY_ABSX:
            {
                uint16_t absAddress = ABSXAddressing(machineCycles, memory);
                Y = ReadByte(machineCycles, absAddress, memory);
                LDSetFlags(Y);
            } break;

            ////////////////////////////////////
            // STA
            ////////////////////////////////////

            case STA_ZP:
            {
                uint8_t address = FetchByte(machineCycles, memory);
                StoreByte(machineCycles, address, A, memory);
            } break;

            case STA_ZPX:
            {
                uint8_t zeroPageAddress = ZPXAddressing(machineCycles, memory);
                StoreByte(machineCycles, zeroPageAddress, A, memory);
            } break;

            case STA_ABS:
            {
                uint16_t absAddress = ABSAddressing(machineCycles, memory);
                StoreByte(machineCycles, absAddress, A, memory);
            } break;

            case STA_ABSX:
            {
                uint16_t absAddress = ABSXAddressing5(machineCycles, memory);
                StoreByte(machineCycles, absAddress, A, memory);
            } break;

            case STA_ABSY:
            {
                uint16_t absAddress = ABSYAddressing5(machineCycles, memory);
                StoreByte(machineCycles, absAddress, A, memory);
            } break;

            case STA_INDX:
            {
                uint16_t target = INDXAddressing(machineCycles, memory);
                StoreByte(machineCycles, target, A, memory);
            } break;

            case STA_INDY:
            {
                uint16_t target = INDYAddressing6(machineCycles, memory);
                StoreByte(machineCycles, target, A, memory);
            } break;

            ////////////////////////////////////
            // STX
            ////////////////////////////////////

            case STX_ZP:
            {
                uint8_t address = FetchByte(machineCycles, memory);
                StoreByte(machineCycles, address, X, memory);
            } break;

            case STX_ZPY:
            {
                uint8_t zeroPageAddress = ZPYAddressing(machineCycles, memory);
                StoreByte(machineCycles, zeroPageAddress, X, memory);
            } break;

            case STX_ABS:
            {
                uint16_t absAddress = ABSAddressing(machineCycles, memory);
                StoreByte(machineCycles, absAddress, X, memory);
            } break;

            ////////////////////////////////////
            // STY
            ////////////////////////////////////

            case STY_ZP:
            {
                uint8_t address = FetchByte(machineCycles, memory);
                StoreByte(machineCycles, address, Y, memory);
            } break;

            case STY_ZPX:
            {
                uint8_t zeroPageAddress = ZPXAddressing(machineCycles, memory);
                StoreByte(machineCycles, zeroPageAddress, Y, memory);
            } break;

            case STY_ABS:
            {
                uint16_t absAddress = ABSAddressing(machineCycles, memory);
                StoreByte(machineCycles, absAddress, Y, memory);
            } break;

            ////////////////////////////////////
            // TAX, TAY, TXA, TYA
            ////////////////////////////////////

            case TAX: X = A; machineCycles--; break;
            case TAY: Y = A; machineCycles--; break;
            case TXA: A = X; machineCycles--; break;
            case TYA: A = Y; machineCycles--; break;

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
                return;
            } break;
        }
    }
}