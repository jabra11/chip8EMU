#include <array>

class Registers
{
public:
    uint8_t get(const uint8_t reg) const;
    void put(const uint8_t reg, const uint8_t val);

private:
    // general purpose 8bit registers
    // V[0x000F] should not be used by any program. It
    // is used by some instructions to set flags.
    std::array<uint8_t, 16> V{};

    // I register is used to store memory addresses
    uint16_t I;

    // special purpose register, when these are != 0
    // they should be decremented at a rate of 60HZ.
    // sound-timer
    uint8_t ST;
    // delay-timer
    uint8_t DT;
};