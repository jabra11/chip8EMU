#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

class Timer
{
public: 
    enum class Type 
    {
        sound, 
        delay
    };

    bool is_ready(Type t, int required_rate)
    {
        if (t == Type::sound)
        {
            if (std::chrono::duration_cast<std::chrono::microseconds>(timer.now() - 
                sound_interval).count() > (1'000'000 / required_rate))
            {
                    return true;
                    sound_interval = timer.now();
            }
            else
                return false;
        }
        else
        {
            if (std::chrono::duration_cast<std::chrono::microseconds>(timer.now() - 
                delay_interval).count() > (1'000'000 / required_rate))
            {
                return true;
                delay_interval = timer.now();
            }
            else
                return false;
        }
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> sound_interval{};
    std::chrono::time_point<std::chrono::steady_clock> delay_interval;
    std::chrono::steady_clock timer;
};

#endif // TIMER_HPP