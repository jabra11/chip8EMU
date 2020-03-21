#include "cpu/CPU.hpp"

class Emulator
{
public:
    Emulator()=default;

private:
    Memory ram;
    CPU cpu{ram};
};