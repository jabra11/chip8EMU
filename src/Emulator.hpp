#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <vector>
#include <array>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Game_selector.hpp"
#include "cpu/CPU.hpp"
#include "memory/Memory.hpp"
#include "display/Display.hpp"
#include "keyboard/Keyboard.hpp"
#include "utility/Timer.hpp"
#include "utility/Object_manager.hpp"

class Emulator
{
public:
    struct CPU_state
    {
        int clockcycles_done = 0;
        int clockspeed = 1000;
        int actual_clockspeed = 1000;
        double cycle_adjustment = 1;

        // set this to true if the emulator can't 
        // emulate faster
        bool bottleneck = false;

        bool halt = false;
        bool step = false;

        bool changing_clockspeed = false;
        bool new_cycle = true;
    };

    explicit Emulator(int w = 1024, int h = 768);
    int run();

private:

    // load font data into the interpreter
    // space of the RAM
    void init_interpreter_space();

    // preloads all labels etc
    void setup_graphics();

    // parameters are in % relative to the window size
    sf::Vector2f get_graphics_pos(int x_percent, int y_percent, 
    int x_offset = 0, int y_offset = 0) const;

    // wipe cpu and display states
    void reset_states();

    void handle_userinput();
    void handle_events();
    void render();
    void update_graphics();

    // preprocess the chip8 display
    // to make it fit the Emulator's screen properly
    sf::RectangleShape& preprocess_display(sf::RectangleShape& obj);

    void update_clockspeed(int percentage);
    void emulate_cpu();
    bool cpu_ready(double hz);

    void inject_rom(const std::string& path, int start_index = 0x200);
    std::vector<uint8_t> get_bytes(const std::string& path);

    sf::RenderWindow window;
    Object_manager manager;
    Game_selector game_selector{&window};
    
    // in hz
    int framerate = 144;

    std::array<std::string, 8> opcodes;
    std::array<std::string, 8> opcode_args;

    bool mouse_pressed = false;

    CPU_state cs;
    Timer timer;
    Memory ram;
    Display display;
    Keyboard keyboard;
    CPU cpu{&ram, &keyboard, &display};
};

#endif // EMULATOR_HPP
