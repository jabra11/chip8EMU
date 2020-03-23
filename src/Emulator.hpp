#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include "cpu/CPU.hpp"
#include "memory/Memory.hpp"

class Emulator
{
public:
    Emulator()=default;

private:
    Memory ram;
    CPU cpu{ram};
};

#endif // EMULATOR_HPP