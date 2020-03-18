#include <array>

class Registers
{
public:
private:
    // general purpose 8bit registers
    // V[0x000F] should not be used by any program. It
    // is used by some instructions to set flags.
    std::array<uint8_t, 16> V;

    // I register is used to store memory addresses
    uint16_t I;

    // special purpose register, when these are != 0
    // they should be decremented at a rate of 60HZ.
    uint8_t sound_timer;
    uint8_t delay_timer;
};