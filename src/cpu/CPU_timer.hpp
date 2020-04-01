#ifndef CPU_TIMER_HPP
#define CPU_TIMER_HPP

#include <chrono>

class CPU_timer
{
public: 
    enum class Type 
    {
        sound, 
        delay
    };

    bool is_ready(Type t, int required_rate)
    {
        if (required_rate == 0)
            return false;
        if (t == Type::sound)
        {
            if (std::chrono::duration_cast<std::chrono::microseconds>(timer.now() - 
                sound_interval).count() > (1'000'000 / required_rate))
            {
                sound_interval = timer.now();
                return true;
            }
            else
                return false;
        }
        else
        {
            if (std::chrono::duration_cast<std::chrono::microseconds>(timer.now() - 
                delay_interval).count() > (1'000'000 / required_rate))
            {
                delay_interval = timer.now();
                return true;
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

#endif // CPU_TIMER_HPP