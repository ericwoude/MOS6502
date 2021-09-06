#ifndef MEM_H
#define MEM_H

#include <array>
#include <cstdint>

class Mem
{
    public:
    void Initialize();
    void WriteWord(uint16_t value, uint32_t address, uint32_t& machine_cycles);
 
    // Enables reading and writing to memory using the [] operator.
    uint8_t operator[](uint32_t address) const;
    uint8_t& operator[](uint32_t address);

    private:
    static const uint32_t max_size = 64 * 1024;
    std::array<uint8_t, max_size> data;
};

#endif // MEM_H