#ifndef REGISTERS_HPP
#define REGISTERS_HPP

#include <array>

class Registers
{
public:
    uint8_t get(uint8_t reg) const;
    void put(uint8_t reg, uint8_t val);

    // I register is used to store memory addresses
    uint16_t I{};
    
    // special purpose register, when these are != 0
    // they should be decremented at a rate of 60HZ.
    // sound-timer
    uint8_t ST{};
    // delay-timer
    uint8_t DT{};
private:

    // general purpose 8bit registers
    // V[0x000F] should not be used by any program. It
    // is used by some instructions to set flags.
    std::array<uint8_t, 16> V{};
};

#endif // REGISTERS_HPP