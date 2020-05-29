#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <array>
#include <cstdint>

class Memory
{
public:
    uint8_t read(uint16_t address) const;
    void write(uint16_t address, uint8_t val);
    
private:
    // 0x0000 - 0x01FF: Interpreter space
    // 0x0200 - 0x0FFF: Program space 
    std::array<uint8_t, 4096> ram{};
};

#endif // MEMORY_HPP