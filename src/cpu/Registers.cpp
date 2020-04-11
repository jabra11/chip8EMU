#include "Registers.hpp"

#include <iostream>


uint8_t Registers::get(uint8_t index) const
{
    if (index < V.size())
        return V[index];
    else
    {
        std::cerr << "Can't access register at pos: " 
                << static_cast<int>(index) << '\n';
        throw std::runtime_error{"Register access out of bound."};
    }
}

void Registers::put(uint8_t index, uint8_t val)
{
    if (index < V.size())
        V[index] = val;
    else
    {
        std::cerr << "Can't access register at pos: " 
                << static_cast<int>(index) << '\n';
        throw std::runtime_error{"Register access out of bound."};
    }
}