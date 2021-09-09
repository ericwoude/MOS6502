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