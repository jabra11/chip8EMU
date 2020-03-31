#include "cpu/CPU.hpp"
#include "memory/Memory.hpp"
#include "Emulator.hpp"

int main()
{
    // http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    
    //Memory mem;
    //CPU cpu{&mem};
    //uint16_t test = 0xF065;
    //cpu.parse_instruction(test);

    Emulator emu;
    emu.inject_rom("../roms/pong.rom");
    emu.run();
    return 0;
}