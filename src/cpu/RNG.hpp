#ifndef RNG_HPP
#define RNG_HPP

#include <random>

class RNG
{
public:
    // inclusive range
    int get(const int min, const int max)
    {
        std::uniform_int_distribution<> dis(min, max);
        return dis(gen);
    }

private:
    std::mt19937 gen{std::random_device{}()};
};


#endif // RNG_HPP