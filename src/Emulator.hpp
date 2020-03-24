#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <vector>

#include "cpu/CPU.hpp"
#include "memory/Memory.hpp"

class Emulator
{
public:
    Emulator()=default;

    void run();
    void inject_rom(const std::string& path, int start_index = 0x200);

private:

    std::vector<uint8_t> get_bytes(const std::string& path);

    Memory ram;
    CPU cpu{&ram};
};

#endif // EMULATOR_HPP