#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>
#include <stdexcept>

class Timer
{
public: 
    enum class Type 
    {
        cpu, 
        framerate,
        seconds,
        milliseconds
    };

    bool is_ready(Type t, long long required_rate)
    {
        if (required_rate == 0)
            return false;
        if (t == Type::cpu)
        {
            if (std::chrono::duration_cast<std::chrono::microseconds>(timer.now() - 
                cpu_interval).count() > (1'000'000 / required_rate))
            {
                cpu_interval = timer.now();
                return true;
            }
            else
                return false;
        }
        else if (t==Type::framerate)
        {
            if (std::chrono::duration_cast<std::chrono::microseconds>(timer.now() - 
                frame_interval).count() > (1'000'000 / required_rate))
            {
                frame_interval = timer.now();
                return true;
            }
            else
                return false;
        }
        else if (t==Type::seconds)
        {
            if (std::chrono::duration_cast<std::chrono::microseconds>(timer.now() - 
                seconds).count() > (1'000'000 / required_rate))
            {
                seconds = timer.now();
                return true;
            }
            else
                return false;
        }
        else if (t==Type::milliseconds)
        {
            if (std::chrono::duration_cast<std::chrono::milliseconds>(timer.now() - 
                milliseconds).count() > (required_rate))
            {
                milliseconds = timer.now();
                return true;
            }
            else
                return false;
        }

        throw std::runtime_error{"unsupported timertype"};
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> milliseconds{};
    std::chrono::time_point<std::chrono::steady_clock> seconds{};
    std::chrono::time_point<std::chrono::steady_clock> frame_interval{};
    std::chrono::time_point<std::chrono::steady_clock> cpu_interval{};

    std::chrono::steady_clock timer;
};

#endif // TIMER_HPP