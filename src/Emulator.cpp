#include <vector>
#include <fstream>
#include <iostream>

#include "Emulator.hpp"

void Emulator::run()
{
    while (true)
        cpu.execute_next_cycle();
}

void Emulator::inject_rom(const std::string &path, int start_index)
{
    auto bytes = get_bytes(path);

    for (size_t i = 0; i < bytes.size(); ++i) 
        ram.write(start_index + i, bytes[i]);
}
   
std::vector<uint8_t> Emulator::get_bytes(const std::string& path)
{
    std::vector<uint8_t> bytes;
    std::ifstream in;
    in.open(path, std::ios::binary);
    if (!in)
        throw std::runtime_error{"Can't find " + path + "."};

    char temp;
    for (in.get(temp); !in.eof(); in.get(temp))
    {
        bytes.emplace_back(static_cast<uint8_t>(temp));
    }
    in.close();
    return bytes;
}