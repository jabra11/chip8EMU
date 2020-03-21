#include "cpu/CPU.hpp"

int main()
{
    // http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    
    Memory mem;
    CPU cpu{mem};
    uint16_t test = 0x8FF6;
    cpu.parse_instruction(test);

    return 0;
}