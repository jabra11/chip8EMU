#include "Registers.hpp"

#include <iostream>


uint8_t Registers::get(const uint8_t index) const
{
    if (index >= 0 && index < V.size())
        return V[index];
    else
    {
        std::cerr << "Can't access register at pos: " 
                << static_cast<int>(index) << '\n';
        throw std::runtime_error{"Register access out of bound."};
    }
}

void Registers::put(const uint8_t index, const uint8_t val)
{
    if (index >= 0 && index < V.size())
        V[index] = val;
    else
    {
        std::cerr << "Can't access register at pos: " 
                << static_cast<int>(index) << '\n';
        throw std::runtime_error{"Register access out of bound."};
    }
}