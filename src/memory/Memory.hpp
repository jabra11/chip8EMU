#include <array>

class Memory
{
public:

private:
    // 0x0000 - 0x01FF: Interpreter space
    // 0x0200 - 0x0FFF: Program space 
    std::array<uint8_t, 4096> ram;
};