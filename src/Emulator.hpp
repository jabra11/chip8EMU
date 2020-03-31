#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <vector>
#include <SFML/Graphics.hpp>

//#include <Timings.hpp>

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

    void setup_graphics();

    // parameters are in % relative to the window size
    sf::Vector2f set_graphics_pos(int x_percent, int y_percent, 
    int x_offset = 0, int y_offset = 0) const;

    void handle_events();
    void render();
    void update_graphics();

    // preprocess the chip8 display
    // to make it fit the Emulator properly
    sf::RectangleShape& preprocess_display(sf::RectangleShape& obj);

    void emulate_cpu();
    bool cpu_ready();
    std::vector<uint8_t> get_bytes(const std::string& path);

    
    sf::RenderWindow window;
    Object_manager manager;

    // in hz
    int clock_speed = 25;

    // keep track of bottlenecking
    int clock_cycles_done = 0;

    Timer timer;
    Memory ram;
    Display display;
    Keyboard keyboard;
    CPU cpu{&ram, &keyboard, &display};
};

#endif // EMULATOR_HPP