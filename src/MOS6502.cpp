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

void Mem::WriteWord(uint16_t value, uint32_t address, uint32_t& machine_cycles)
{
    data[address] = value & 0xFF;
    data[address + 1] = value >> 8;

    machine_cycles -= 2;
}

/**************
 * CPU
 **************/

#define DEFINE_OPCODE(HEX, NAME, ADDRESSING_MODE)         \
    instruction.addr = &CPU::Addr##ADDRESSING_MODE; \
    instruction.op = &CPU::Op##NAME;               \
    dispatch_table[HEX] = instruction

void CPU::Reset(Mem& memory)
{
    // Populate dispatch table
    Instruction instruction;

    // Prefill dispatch table with illegal opcode handlers
    instruction.addr = &CPU::AddrImplicit;
    instruction.op = &CPU::OpIllegal;
    dispatch_table.fill(instruction);

    // Actual data for dispatch 
    // LOAD & STORE
    DEFINE_OPCODE(0xA9, LDA, Immediate);
    DEFINE_OPCODE(0xA5, LDA, ZeroPage);
    DEFINE_OPCODE(0xB5, LDA, ZeroPageX);
    DEFINE_OPCODE(0x6D, LDA, Absolute);
    DEFINE_OPCODE(0xBD, LDA, AbsoluteX);
    DEFINE_OPCODE(0xB9, LDA, AbsoluteY);
    DEFINE_OPCODE(0xA1, LDA, IndexedIndirect);
    DEFINE_OPCODE(0xB1, LDA, IndirectIndexed);

    DEFINE_OPCODE(0xA2, LDX, Immediate);
    DEFINE_OPCODE(0xA6, LDX, ZeroPage);
    DEFINE_OPCODE(0xB6, LDX, ZeroPageY);
    DEFINE_OPCODE(0xAE, LDX, Absolute);
    DEFINE_OPCODE(0xBE, LDX, AbsoluteY);

    DEFINE_OPCODE(0xA0, LDY, Immediate);
    DEFINE_OPCODE(0xA4, LDY, ZeroPage);
    DEFINE_OPCODE(0xB4, LDY, ZeroPageX);
    DEFINE_OPCODE(0xAC, LDY, Absolute);
    DEFINE_OPCODE(0xBC, LDY, AbsoluteX);

    DEFINE_OPCODE(0x85, STA, ZeroPage);
    DEFINE_OPCODE(0x95, STA, ZeroPageX);
    DEFINE_OPCODE(0x8D, STA, Absolute);
    DEFINE_OPCODE(0x9D, STA, AbsoluteX);
    DEFINE_OPCODE(0x99, STA, AbsoluteY);
    DEFINE_OPCODE(0x81, STA, IndexedIndirect);
    DEFINE_OPCODE(0x91, STA, IndirectIndexed);

    DEFINE_OPCODE(0x86, STX, ZeroPage);
    DEFINE_OPCODE(0x96, STX, ZeroPageY);
    DEFINE_OPCODE(0x8e, STX, Absolute);

    DEFINE_OPCODE(0x84, STY, ZeroPage);
    DEFINE_OPCODE(0x94, STY, ZeroPageX);
    DEFINE_OPCODE(0x8C, STY, Absolute);

    // REGISTER TRANSFERS
    DEFINE_OPCODE(0xAA, TAX, Implied);
    DEFINE_OPCODE(0xA8, TAY, Implied);
    DEFINE_OPCODE(0x8A, TXA, Implied);
    DEFINE_OPCODE(0x98, TYA, Implied);

    // STACK OPERATIONS 
    DEFINE_OPCODE(0xBA, TSX, Implied);
    DEFINE_OPCODE(0x9A, TXS, Implied);
    DEFINE_OPCODE(0x48, PHA, Implied);
    DEFINE_OPCODE(0x08, PHP, Implied);
    DEFINE_OPCODE(0x68, PLA, Implied);
    DEFINE_OPCODE(0x28, PLP, Implied);

    PC = 0xFFFC;
    SP = 0xFF;

    // Clear processor status flags
    C = 0; Z = 0; I = 0; D = 0; B = 0; V = 0; N = 0;

    // Reset general-purpose registers
    A = 0; X = 0; Y = 0;

    // Fill memory with 0s.
    memory.Initialize();
}

// Fetch a single byte from memory offsetted by the PC.
uint8_t CPU::FetchByte(uint32_t& machine_cycles, Mem& memory)
{
    uint8_t b = memory[PC];
    PC++;
    machine_cycles--;

    return b;
}

uint16_t CPU::FetchWord(uint32_t& machine_cycles, Mem& memory)
{
    uint16_t w = memory[PC];
    w |= (memory[PC + 1] << 8);

    PC += 2;
    machine_cycles -=2;

    return w;
}

// Like FetchByte, except it fetches using an address and it does not
// increment the program counter.
uint8_t CPU::ReadByte(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    uint8_t b = memory[address];
    machine_cycles--;

    return b;
}

void CPU::StoreByte(uint32_t& machine_cycles, uint16_t address, uint8_t value, Mem& memory)
{
    memory[address] = value;
    machine_cycles--;
}

// void CPU::StoreWord(uint32_t& machine_cycles, uint16_t address, uint16_t value, Mem& memory)
// {
//     memory[address]     = (uint8_t) value;
//     memory[address + 1] = (value << 8);

//     machine_cycles -= 2;
// }

uint16_t CPU::ReadWord(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    uint8_t l = ReadByte(machine_cycles, address, memory);
    uint8_t h = ReadByte(machine_cycles, address + 1, memory);

    return (h << 8) | l;
}

void CPU::PushToStack(uint32_t& machine_cycles, uint8_t value, Mem& memory)
{
    StoreByte(machine_cycles, 0x100 + SP--, value, memory);
}

uint8_t CPU::PullFromStack(uint32_t& machine_cycles, Mem& memory)
{
    SP++;
    machine_cycles -= 3; // Why does this consume three cycles (four total)?

    return ReadByte(machine_cycles, 0x100 + SP, memory);
}

// Addressing modes
uint8_t CPU::ImmediateAddressing(uint32_t& machine_cycles, Mem& memory)
{
    return FetchByte(machine_cycles, memory);
}

uint8_t CPU::ZPAddressing(uint32_t& machine_cycles, Mem& memory)
{
    return FetchByte(machine_cycles, memory);
}

uint8_t CPU::ZPXAddressing(uint32_t& machine_cycles, Mem& memory)
{
    uint8_t zeroPageAddress = FetchByte(machine_cycles, memory);

    // If it exceeds the zero page, wrap around.
    uint8_t zeroPageAddressXWrap = (zeroPageAddress + X) & 0xFF;
    machine_cycles--;

    return zeroPageAddressXWrap;
}

uint8_t CPU::ZPYAddressing(uint32_t& machine_cycles, Mem& memory)
{
    uint8_t zeroPageAddress = FetchByte(machine_cycles, memory);

    // If it exceeds the zero page, wrap around.
    uint8_t zeroPageAddressYWrap = (zeroPageAddress + Y) & 0xFF;
    machine_cycles--;

    return zeroPageAddressYWrap;
}

uint16_t CPU::ABSAddressing(uint32_t& machine_cycles, Mem& memory)
{
    return FetchWord(machine_cycles, memory);
}

uint16_t CPU::ABSXAddressing(uint32_t& machine_cycles, Mem& memory)
{
    uint16_t absAddress = FetchWord(machine_cycles, memory);
    uint16_t sum = absAddress + X;

    // Newly computed address crossed the page boundary.
    if (sum - absAddress >= 0xFF)
        machine_cycles--;

    return sum;
}

uint16_t CPU::ABSXAddressing5(uint32_t& machine_cycles, Mem& memory)
{
    uint16_t absAddress = FetchWord(machine_cycles, memory);
    uint16_t sum = absAddress + X;
    machine_cycles--;

    return sum;
}

uint16_t CPU::ABSYAddressing(uint32_t& machine_cycles, Mem& memory)
{
    uint16_t absAddress = FetchWord(machine_cycles, memory);
    uint16_t sum = absAddress + Y;

    // Newly computed address crossed the page boundary.
    if (sum - absAddress >= 0xFF)
        machine_cycles--;
    
    return sum;
}

uint16_t CPU::ABSYAddressing5(uint32_t& machine_cycles, Mem& memory)
{
    uint16_t absAddress = FetchWord(machine_cycles, memory);
    uint16_t sum = absAddress + Y;
    machine_cycles--;

    return sum;
}

uint16_t CPU::INDXAddressing(uint32_t& machine_cycles, Mem& memory)
{
    uint16_t address = FetchByte(machine_cycles, memory);
    uint16_t target = ReadWord(machine_cycles, (address + X) & 0xFF, memory);
    machine_cycles--;

    return target;
}

uint16_t CPU::INDYAddressing(uint32_t& machine_cycles, Mem& memory)
{
    uint8_t zeroPageAddress = FetchByte(machine_cycles, memory);
    uint16_t target = ReadWord(machine_cycles, zeroPageAddress, memory);
    uint16_t targetY = target + Y;

    if (targetY - target >= 0xFF)
            machine_cycles--;

    return targetY;
}

uint16_t CPU::INDYAddressing6(uint32_t& machine_cycles, Mem& memory)
{
    uint8_t zeroPageAddress = FetchByte(machine_cycles, memory);
    uint16_t target = ReadWord(machine_cycles, zeroPageAddress, memory);
    uint16_t targetY = target + Y;
    machine_cycles--;

    return targetY;
}


// Instruction specific functions
void CPU::LDSetFlags(uint8_t reg)
{
    Z = (reg == 0);
    N = (reg & 0b1000000) > 0;
}

void CPU::exec_instruction(Instruction instruction, uint32_t machine_cycles, Mem& memory)
{
    uint16_t address = (this->*instruction.addr)(machine_cycles, memory);
    (this->*instruction.op)(machine_cycles, address, memory);
}

// dispatch table
void CPU::Execute(uint32_t machine_cycles, Mem& memory)
{
    while (machine_cycles > 0)
    {
        uint8_t opcode = FetchByte(machine_cycles, memory);
        Instruction instruction = dispatch_table[opcode];
        exec_instruction(instruction, machine_cycles, memory);
    }
}

// switch-based
void CPU::Execute2(uint32_t machine_cycles, Mem& memory)
{
    while (machine_cycles > 0)
    {
        uint8_t instruction = FetchByte(machine_cycles, memory);

        switch (instruction)
        {
            ////////////////////////////////////
            // LDA
            ////////////////////////////////////

            case LDA_IM:
            {
                A = ImmediateAddressing(machine_cycles, memory);
                LDSetFlags(A);
            } break;

            // Fetches zero page address from instruction,
            // then uses zero page address to read a byte from
            // the zero page.
            case LDA_ZP:
            {
                uint8_t zeroPageAddress = ZPAddressing(machine_cycles, memory);
                A = ReadByte(machine_cycles, zeroPageAddress, memory);
                LDSetFlags(A);
            } break;

            // Like LDA_ZP, except it adds the contents of register X
            // to the zero page address. If zeroPageAddress + X exceeds
            // the zero page size (FF), then it will wrap around.
            case LDA_ZPX:
            {
                uint8_t zeroPageAddress = ZPXAddressing(machine_cycles, memory);
                A = ReadByte(machine_cycles, zeroPageAddress, memory);
                LDSetFlags(A);
            } break;

            // Fetches an absolute address (word) from the PC by
            // concatenating two bytes. Then reads the byte at the
            // absolute address.
            case LDA_ABS:
            {
                uint16_t absAddress = ABSAddressing(machine_cycles, memory);
                A = ReadByte(machine_cycles, absAddress, memory);
                LDSetFlags(A);
            } break;

            // Like LDA_ABS, except adds the contents of register X
            // to the absolute address.
            case LDA_ABSX:
            {
                uint16_t absAddress = ABSXAddressing(machine_cycles, memory);
                A = ReadByte(machine_cycles, absAddress, memory);
                LDSetFlags(A);
            } break;

            // Like LDA_ABSX, except it uses the Y register instead of
            // the X register.
            case LDA_ABSY:
            {
                uint16_t absAddress = ABSYAddressing(machine_cycles, memory);
                A = ReadByte(machine_cycles, absAddress, memory);
                LDSetFlags(A);
            } break;

            // Fetches an address, adds the contents of X to it, and
            // uses the sum to read the effective address from the zero page.
            // Finally, it reads a byte into A from memory ofsetted by the
            // effective address.
            case LDA_INDX:
            {
                uint16_t target = INDXAddressing(machine_cycles, memory);
                A = ReadByte(machine_cycles, target, memory);
                LDSetFlags(A);
            } break;

            // Fetches address from memory. Fetches a new target address
            // from memory using the old address. Reads the memory at
            // target + general register Y. Consumes an extra machine
            // cycle if the memory is read out of page bound.
            case LDA_INDY:
            {
                uint16_t target = INDYAddressing(machine_cycles, memory);
                A = ReadByte(machine_cycles, target, memory);
                LDSetFlags(A);
            } break;

            ////////////////////////////////////
            // LDX
            ////////////////////////////////////

            case LDX_IM:
            {
                X = ImmediateAddressing(machine_cycles, memory);
                LDSetFlags(X);
            } break;

            case LDX_ZP:
            {
                uint8_t zeroPageAddress = ZPAddressing(machine_cycles, memory);
                X = ReadByte(machine_cycles, zeroPageAddress, memory);
                LDSetFlags(X);
            } break;

            case LDX_ZPY:
            {
                uint8_t zeroPageAddress = ZPYAddressing(machine_cycles, memory);
                X = ReadByte(machine_cycles, zeroPageAddress, memory);
                LDSetFlags(X);
            } break;

            case LDX_ABS:
            {
                uint16_t absAddress = ABSAddressing(machine_cycles, memory);
                X = ReadByte(machine_cycles, absAddress, memory);
                LDSetFlags(X);
            } break;

            case LDX_ABSY:
            {
                uint16_t absAddress = ABSYAddressing(machine_cycles, memory);
                X = ReadByte(machine_cycles, absAddress, memory);
                LDSetFlags(X);
            } break;

            ////////////////////////////////////
            // LDY
            ////////////////////////////////////

            case LDY_IM:
            {
                Y = ImmediateAddressing(machine_cycles, memory);
                LDSetFlags(Y);
            } break;

            case LDY_ZP:
            {
                uint8_t zeroPageAddress = ZPAddressing(machine_cycles, memory);
                Y = ReadByte(machine_cycles, zeroPageAddress, memory);
                LDSetFlags(Y);
            } break;

            case LDY_ZPX:
            {
                uint8_t zeroPageAddress = ZPXAddressing(machine_cycles, memory);
                Y = ReadByte(machine_cycles, zeroPageAddress, memory);
                LDSetFlags(Y);
            } break;

            case LDY_ABS:
            {
                uint16_t absAddress = ABSAddressing(machine_cycles, memory);
                Y = ReadByte(machine_cycles, absAddress, memory);
                LDSetFlags(Y);
            } break;

            case LDY_ABSX:
            {
                uint16_t absAddress = ABSXAddressing(machine_cycles, memory);
                Y = ReadByte(machine_cycles, absAddress, memory);
                LDSetFlags(Y);
            } break;

            ////////////////////////////////////
            // STA
            ////////////////////////////////////

            case STA_ZP:
            {
                uint8_t address = FetchByte(machine_cycles, memory);
                StoreByte(machine_cycles, address, A, memory);
            } break;

            case STA_ZPX:
            {
                uint8_t zeroPageAddress = ZPXAddressing(machine_cycles, memory);
                StoreByte(machine_cycles, zeroPageAddress, A, memory);
            } break;

            case STA_ABS:
            {
                uint16_t absAddress = ABSAddressing(machine_cycles, memory);
                StoreByte(machine_cycles, absAddress, A, memory);
            } break;

            case STA_ABSX:
            {
                uint16_t absAddress = ABSXAddressing5(machine_cycles, memory);
                StoreByte(machine_cycles, absAddress, A, memory);
            } break;

            case STA_ABSY:
            {
                uint16_t absAddress = ABSYAddressing5(machine_cycles, memory);
                StoreByte(machine_cycles, absAddress, A, memory);
            } break;

            case STA_INDX:
            {
                uint16_t target = INDXAddressing(machine_cycles, memory);
                StoreByte(machine_cycles, target, A, memory);
            } break;

            case STA_INDY:
            {
                uint16_t target = INDYAddressing6(machine_cycles, memory);
                StoreByte(machine_cycles, target, A, memory);
            } break;

            ////////////////////////////////////
            // STX
            ////////////////////////////////////

            case STX_ZP:
            {
                uint8_t address = FetchByte(machine_cycles, memory);
                StoreByte(machine_cycles, address, X, memory);
            } break;

            case STX_ZPY:
            {
                uint8_t zeroPageAddress = ZPYAddressing(machine_cycles, memory);
                StoreByte(machine_cycles, zeroPageAddress, X, memory);
            } break;

            case STX_ABS:
            {
                uint16_t absAddress = ABSAddressing(machine_cycles, memory);
                StoreByte(machine_cycles, absAddress, X, memory);
            } break;

            ////////////////////////////////////
            // STY
            ////////////////////////////////////

            case STY_ZP:
            {
                uint8_t address = FetchByte(machine_cycles, memory);
                StoreByte(machine_cycles, address, Y, memory);
            } break;

            case STY_ZPX:
            {
                uint8_t zeroPageAddress = ZPXAddressing(machine_cycles, memory);
                StoreByte(machine_cycles, zeroPageAddress, Y, memory);
            } break;

            case STY_ABS:
            {
                uint16_t absAddress = ABSAddressing(machine_cycles, memory);
                StoreByte(machine_cycles, absAddress, Y, memory);
            } break;

            ////////////////////////////////////
            // TAX, TAY, TXA, TYA
            ////////////////////////////////////

            case TAX: X = A; machine_cycles--; break;
            case TAY: Y = A; machine_cycles--; break;
            case TXA: A = X; machine_cycles--; break;
            case TYA: A = Y; machine_cycles--; break;

            ////////////////////////////////////
            // STACK OPERATIONS
            ////////////////////////////////////

            case TSX:
            {
                X = SP;
                machine_cycles--;
                LDSetFlags(X);
            } break;

            case TXS:
            {
                SP = X;
                machine_cycles--;
            } break;

            case PHA:
            {
                PushToStack(machine_cycles, A, memory);
            } break;

            case PHP:
            {
                PushToStack(machine_cycles, PS, memory);
            } break;

            case PLA:
            {
                A = PullFromStack(machine_cycles, memory);
                LDSetFlags(A);
            } break;

            case PLP:
            {
                PS = PullFromStack(machine_cycles, memory);
            } break;

            // case INS_JPS_A:
            // {
            //     uint16_t address = FetchWord(machine_cycles, memory);
            //     memory.WriteWord(PC - 1, SP++, machine_cycles);

            //     PC = address;
            //     machine_cycles--;
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

// Addressing mode functions
uint16_t CPU::AddrImplied(uint32_t&, Mem&)
{
    return 0;
}

uint16_t CPU::AddrImplicit(uint32_t&, Mem&)
{
    return 0;
}

uint16_t CPU::AddrImmediate(uint32_t&, Mem&)
{
    return PC++;
}

uint16_t CPU::AddrZeroPage(uint32_t& machine_cycles, Mem& memory)
{
    return FetchByte(machine_cycles, memory);
}

uint16_t CPU::AddrZeroPageX(uint32_t& machine_cycles, Mem& memory)
{
    uint8_t zeroPageAddress = FetchByte(machine_cycles, memory);

    // If it exceeds the zero page, wrap around.
    uint8_t zeroPageAddressXWrap = (zeroPageAddress + X) & 0xFF;
    machine_cycles--;

    return zeroPageAddressXWrap;
}

uint16_t CPU::AddrZeroPageY(uint32_t& machine_cycles, Mem& memory)
{
    uint8_t zeroPageAddress = FetchByte(machine_cycles, memory);

    // If it exceeds the zero page, wrap around.
    uint8_t zeroPageAddressYWrap = (zeroPageAddress + Y) & 0xFF;
    machine_cycles--;

    return zeroPageAddressYWrap;
}

uint16_t CPU::AddrAbsolute(uint32_t& machine_cycles, Mem& memory)
{
    return FetchWord(machine_cycles, memory);
}

uint16_t CPU::AddrAbsoluteX(uint32_t& machine_cycles, Mem& memory)
{
    uint16_t absAddress = FetchWord(machine_cycles, memory);
    uint16_t sum = absAddress + X;

    // Newly computed address crossed the page boundary.
    if (sum - absAddress >= 0xFF)
        machine_cycles--;

    return sum;
}

uint16_t CPU::AddrAbsoluteX5(uint32_t& machine_cycles, Mem& memory)
{
    uint16_t absAddress = FetchWord(machine_cycles, memory);
    uint16_t sum = absAddress + X;
    machine_cycles--;

    return sum;
}

uint16_t CPU::AddrAbsoluteY(uint32_t& machine_cycles, Mem& memory)
{
    uint16_t absAddress = FetchWord(machine_cycles, memory);
    uint16_t sum = absAddress + Y;

    // Newly computed address crossed the page boundary.
    if (sum - absAddress >= 0xFF)
        machine_cycles--;
    
    return sum;
}

uint16_t CPU::AddrAbsoluteY5(uint32_t& machine_cycles, Mem& memory)
{
    uint16_t absAddress = FetchWord(machine_cycles, memory);
    uint16_t sum = absAddress + Y;
    machine_cycles--;

    return sum;
}

uint16_t CPU::AddrIndexedIndirect(uint32_t& machine_cycles, Mem& memory)
{
    uint16_t address = FetchByte(machine_cycles, memory);
    uint16_t target = ReadWord(machine_cycles, (address + X) & 0xFF, memory);
    machine_cycles--;

    return target;
}

uint16_t CPU::AddrIndirectIndexed(uint32_t& machine_cycles, Mem& memory)
{
    uint8_t zeroPageAddress = FetchByte(machine_cycles, memory);
    uint16_t target = ReadWord(machine_cycles, zeroPageAddress, memory);
    uint16_t targetY = target + Y;

    if (targetY - target >= 0xFF)
            machine_cycles--;

    return targetY;
}

uint16_t CPU::AddrIndirectIndexed6(uint32_t& machine_cycles, Mem& memory)
{
    uint8_t zeroPageAddress = FetchByte(machine_cycles, memory);
    uint16_t target = ReadWord(machine_cycles, zeroPageAddress, memory);
    uint16_t targetY = target + Y;
    machine_cycles--;

    return targetY;
}

// Instruction functions
void CPU::OpLDA(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    A = ReadByte(machine_cycles, address, memory);
    LDSetFlags(A);
}

void CPU::OpLDX(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    X = ReadByte(machine_cycles, address, memory);
    LDSetFlags(X);
}

void CPU::OpLDY(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    Y = ReadByte(machine_cycles, address, memory);
    LDSetFlags(Y);
}

void CPU::OpSTA(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    StoreByte(machine_cycles, address, A, memory);
}

void CPU::OpSTX(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    StoreByte(machine_cycles, address, X, memory);
}

void CPU::OpSTY(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    StoreByte(machine_cycles, address, Y, memory);
}

void CPU::OpTAX(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    X = A;
    machine_cycles--;
    LDSetFlags(X);
}
void CPU::OpTAY(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    Y = A;
    machine_cycles--;
    LDSetFlags(Y);

}
void CPU::OpTXA(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    A = X;
    machine_cycles--;
    LDSetFlags(A);

}
void CPU::OpTYA(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    A = Y;
    machine_cycles--;
    LDSetFlags(A);
}

void CPU::OpTSX(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    X = SP;
    machine_cycles--;
    LDSetFlags(X);
}

void CPU::OpTXS(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    SP = X;
    machine_cycles--;
}

void CPU::OpPHA(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    PushToStack(machine_cycles, A, memory);
}

void CPU::OpPHP(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    PushToStack(machine_cycles, PS, memory);
}

void CPU::OpPLA(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    A = PullFromStack(machine_cycles, memory);
    LDSetFlags(A);
}

void CPU::OpPLP(uint32_t& machine_cycles, uint16_t address, Mem& memory)
{
    PS = PullFromStack(machine_cycles, memory);
}


void CPU::OpIllegal(uint32_t&, uint16_t, Mem&)
{
    std::cout << "Unknown instruction: 0x" << std::endl;
}