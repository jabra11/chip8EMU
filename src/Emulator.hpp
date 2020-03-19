#include "cpu/CPU.hpp"
#include "memory/Memory.hpp"

class Emulator
{
public:
    Emulator()=default;

private:
    CPU cpu;
    Memory ram;
}