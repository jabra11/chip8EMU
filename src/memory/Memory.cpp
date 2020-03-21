#include "Memory.hpp"

uint8_t Memory::read(const uint8_t address) const
{
    // maybe implement checking here
    return ram[address];
}

void Memory::write(const uint8_t address, const uint8_t val)
{
    // maybe implement checking here
    ram[address] = val;
}