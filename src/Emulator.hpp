#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <vector>
#include <SFML/Graphics.hpp>

#include "cpu/CPU.hpp"
#include "memory/Memory.hpp"
#include "display/Display.hpp"
#include "keyboard/Keyboard.hpp"
#include "cpu/Timer.hpp"
#include "Object_manager.hpp"

class Emulator
{
public:
    explicit Emulator(int w = 1024, int h = 768);

    void run();
    void inject_rom(const std::string& path, int start_index = 0x200);

private:

    void handle_events();
    void render();
    void update_graphics();

    void emulate_cpu();
    bool cpu_ready();
    std::vector<uint8_t> get_bytes(const std::string& path);

    
    sf::RenderWindow window;
    Object_manager manager;

    // in hz
    int clock_speed = 20;

    // keep track of bottlenecking
    int clock_cycles_done = 0;

    Timer timer;
    Memory ram;
    Display display;
    Keyboard kb;
    CPU cpu{&ram, &kb, &display};
};

#endif // EMULATOR_HPP