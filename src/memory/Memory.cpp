#include "Memory.hpp"

uint8_t Memory::read(uint16_t address) const
{
    // maybe implement checking here
    return ram[address];
}

void Memory::write(uint16_t address, uint8_t val)
{
    // maybe implement checking here
    ram[address] = val;
}