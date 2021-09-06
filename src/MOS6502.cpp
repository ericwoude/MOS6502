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
    instruction.addr = &CPU::AddrImplied;
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

// Instruction specific functions
void CPU::LDSetFlags(uint8_t reg)
{
    Z = (reg == 0);
    N = (reg & 0b1000000) > 0;
}

void CPU::exec_instruction(Instruction instruction, uint32_t& machine_cycles, Mem& memory)
{
    uint16_t address = (this->*instruction.addr)(machine_cycles, memory);
    (this->*instruction.op)(machine_cycles, address, memory);
}

uint32_t CPU::Execute(uint32_t machine_cycles, Mem& memory)
{
    const uint32_t machine_cycles_requested = machine_cycles;
    while (machine_cycles > 0)
    {
        uint8_t instruction = FetchByte(machine_cycles, memory);
        Instruction ins = dispatch_table[instruction];
        exec_instruction(ins, machine_cycles, memory);
    }

    const uint32_t machine_cycles_used = machine_cycles_requested - machine_cycles;
	return machine_cycles_used;
}

// Addressing mode functions
uint16_t CPU::AddrImplied(uint32_t&, Mem&)
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

    // If the zero page is crossed
    if ((absAddress ^ sum) >> 8)
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

    // If the zero page is crossed
    if ((absAddress ^ sum) >> 8)
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

    // If the zero page is crossed
    if ((target ^ targetY) >> 8)
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
    exit(0);
}