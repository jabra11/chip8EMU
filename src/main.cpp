#include "cpu/CPU.hpp"
#include "memory/Memory.hpp"
#include "Emulator.hpp"

int main()
{
    Emulator emu;
    emu.inject_rom("../roms/pdemo.ch8");
    return emu.run();
}