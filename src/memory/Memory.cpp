#include "Memory.hpp"

#include <cstdint>
#include <stdexcept>

uint8_t Memory::read(uint16_t address) const
{
    if (address < 0xFFF)
        return ram[address];
    else
        throw std::runtime_error{"Invalid address."};
}

void Memory::write(uint16_t address, uint8_t val)
{
    if (address < 0xFFF)
        ram[address] = val;
    else
        throw std::runtime_error{"Invalid address."};
}