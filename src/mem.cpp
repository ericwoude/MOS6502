#include "mem.h"

#include <cassert>

void Mem::Initialize()
{
    data.fill(0);
}

// Read a single byte from memory.
uint8_t Mem::operator[](uint32_t address) const
{
    assert (address <= max_size);
    return data[address];
}

// Write a single byte to memory.
uint8_t& Mem::operator[](uint32_t address)
{
    assert (address <= max_size);
    return data[address];
}

void Mem::WriteWord(uint16_t value, uint32_t address, uint32_t& machine_cycles)
{
    data[address] = value & 0xFF;
    data[address + 1] = value >> 8;

    machine_cycles -= 2;
}
