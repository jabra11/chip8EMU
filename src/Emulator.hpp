#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <vector>
#include <array>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

//#include <Timings.hpp>

#include "cpu/CPU.hpp"
#include "memory/Memory.hpp"
#include "display/Display.hpp"
#include "keyboard/Keyboard.hpp"
#include "utility/Timer.hpp"
#include "utility/Object_manager.hpp"

class Emulator
{
public:
    explicit Emulator(int w = 1024, int h = 768);

    int run();
    void inject_rom(const std::string& path, int start_index = 0x200);

private:

    // load font data into the interpreter
    // space of the RAM
    void init_interpreter_space();

    void setup_graphics();

    // parameters are in % relative to the window size
    sf::Vector2f get_graphics_pos(int x_percent, int y_percent, 
    int x_offset = 0, int y_offset = 0) const;

    void handle_userinput();
    void handle_events();
    void render();
    void update_graphics();

    // make a sound
    void buzz();

    // preprocess the chip8 display
    // to make it fit the Emulator's screen properly
    sf::RectangleShape& preprocess_display(sf::RectangleShape& obj);

    void update_clockspeed(int percentage);

    void emulate_cpu();
    bool cpu_ready(int interval);
    std::vector<uint8_t> get_bytes(const std::string& path);
    
    sf::RenderWindow window;
    Object_manager manager;
    
    // in hz
    int framerate = 144;
    // in hz
    int clock_speed = 1000; 
    double interval = clock_speed;
    // keep track of bottlenecking
    int clock_cycles_done = 0;
    std::array<std::string, 8> opcodes;

    bool mouse_pressed = false;
    bool new_cycle = true;

    // annoying that I have to declare
    // the soundbuffer as a member aswell..
    sf::SoundBuffer sb;
    sf::Sound sound;

    Timer timer;
    Memory ram;
    Display display;
    Keyboard keyboard;
    CPU cpu{&ram, &keyboard, &display};
};

#endif // EMULATOR_HPP