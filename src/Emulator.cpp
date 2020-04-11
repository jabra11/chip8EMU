#include <vector>
#include <fstream>
#include <iostream>
#include <chrono> // sleep_for helper
#include <thread> // sleep_for
#include <sstream>
#include <numeric>

#include <cstddef>


#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Emulator.hpp"
#include "utility/Timer.hpp"
#include "utility/Object_manager.hpp"

Emulator::Emulator(int w, int h)
    :window{sf::VideoMode(w, h), "Chip8EMU"}
{
    setup_graphics();
    init_interpreter_space();

    if (auto g = game_selector.get(); !g.empty())
        inject_rom(g);
}

int Emulator::run()
{
    while (window.isOpen())
    {
        handle_events();
        handle_userinput();
        emulate_cpu();

        if(timer.is_ready(Timer::Type::framerate, framerate))
        {
            update_graphics();
            render();
        }
    }
    return 0;
}

void Emulator::inject_rom(const std::string &path, int start_index)
{
    reset_states();

    auto bytes = get_bytes(path);
    for (size_t i = 0; i < bytes.size(); ++i) 
        ram.write(start_index + i, bytes[i]);
}

void Emulator::init_interpreter_space()
{
    // inject hardcoded font encoding from 
    // http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#2.4
    // into the RAM
    
    // I'll start at address 0x100
    int a = 0x100;

    // 0
    ram.write(a++, 0xF0);
    ram.write(a++, 0x90);
    ram.write(a++, 0x90);
    ram.write(a++, 0x90);
    ram.write(a++, 0xF0);

    // 1
    ram.write(a++, 0x20);
    ram.write(a++, 0x60);
    ram.write(a++, 0x20);
    ram.write(a++, 0x20);
    ram.write(a++, 0x70);

    // 2
    ram.write(a++, 0xF0);
    ram.write(a++, 0x10);
    ram.write(a++, 0xF0);
    ram.write(a++, 0x80);
    ram.write(a++, 0xF0);

    // 3
    ram.write(a++, 0xF0);
    ram.write(a++, 0x10);
    ram.write(a++, 0xF0);
    ram.write(a++, 0x10);
    ram.write(a++, 0xF0);

    // 4
    ram.write(a++, 0x90);
    ram.write(a++, 0x90);
    ram.write(a++, 0xF0);
    ram.write(a++, 0x10);
    ram.write(a++, 0x10);

    // 5
    ram.write(a++, 0xF0);
    ram.write(a++, 0x80);
    ram.write(a++, 0xF0);
    ram.write(a++, 0x10);
    ram.write(a++, 0xF0);

    // 6
    ram.write(a++, 0xF0);
    ram.write(a++, 0x80);
    ram.write(a++, 0xF0);
    ram.write(a++, 0x90);
    ram.write(a++, 0xF0);

    // 7
    ram.write(a++, 0xF0);
    ram.write(a++, 0x10);
    ram.write(a++, 0x20);
    ram.write(a++, 0x40);
    ram.write(a++, 0x40);

    // 8
    ram.write(a++, 0xF0);
    ram.write(a++, 0x90);
    ram.write(a++, 0xF0);
    ram.write(a++, 0x90);
    ram.write(a++, 0xF0);

    // 9
    ram.write(a++, 0xF0);
    ram.write(a++, 0x90);
    ram.write(a++, 0xF0);
    ram.write(a++, 0x10);
    ram.write(a++, 0xF0);

    // A
    ram.write(a++, 0xF0);
    ram.write(a++, 0x90);
    ram.write(a++, 0xF0);
    ram.write(a++, 0x90);
    ram.write(a++, 0x90);

    // B
    ram.write(a++, 0xE0);
    ram.write(a++, 0x90);
    ram.write(a++, 0xE0);
    ram.write(a++, 0x90);
    ram.write(a++, 0xE0);

    // C
    ram.write(a++, 0xF0);
    ram.write(a++, 0x80);
    ram.write(a++, 0x80);
    ram.write(a++, 0x80);
    ram.write(a++, 0xF0);

    // D
    ram.write(a++, 0xE0);
    ram.write(a++, 0x90);
    ram.write(a++, 0x90);
    ram.write(a++, 0x90);
    ram.write(a++, 0xE0);

    // E
    ram.write(a++, 0xF0);
    ram.write(a++, 0x80);
    ram.write(a++, 0xF0);
    ram.write(a++, 0x80);
    ram.write(a++, 0xF0);

    // F
    ram.write(a++, 0xF0);
    ram.write(a++, 0x80);
    ram.write(a++, 0xF0);
    ram.write(a++, 0x80);
    ram.write(a++, 0x80);
}

void Emulator::setup_graphics()
{
    using P = Object_manager::Object_property;

    // add call stack
    for (int i = 0; i < 8; ++i)
    {
        P tmp;
        tmp.name = "S" + std::to_string(i);
        tmp.pos = get_graphics_pos(10, i * 3, 0, 65);
        manager.add_text(tmp);
    }
    for (int i = 7; i < 16; ++i)
    {
        P tmp;
        tmp.name = "S" + std::to_string(i);
        tmp.pos = get_graphics_pos(25, (i - 8) * 3, 0, 65);
        manager.add_text(tmp);
    }

    // add clockspeed_slider_p1
    P tmp;
    tmp.name = "clockspeed_slider1";
    tmp.dim = get_graphics_pos(15, 1);
    tmp.pos = get_graphics_pos(45, 5);
    manager.add_rectangle(tmp);

    // add clockspeed_slider_p2
    tmp = {};
    tmp.name = "clockspeed_slider2";
    tmp.dim = get_graphics_pos(2, 4);
    tmp.pos = get_graphics_pos(45, 5);
    tmp.color = sf::Color::Red;
    manager.add_rectangle(tmp);

    // add clockspeed
    tmp = {};
    tmp.name = "clockspeed";
    tmp.pos = get_graphics_pos(20, 3);
    tmp.color = sf::Color::Green;
    tmp.font_size = 20;
    manager.add_text(tmp);

    // add hold "button"
    tmp = {};
    tmp.name = "HALT";
    tmp.pos = get_graphics_pos(60, 3);
    tmp.color = sf::Color::Green;
    tmp.font_size = 20;
    manager.add_text(tmp);

    // add step "button"
    tmp = {};
    tmp.name = "STEP";
    tmp.pos = get_graphics_pos(68, 3);
    tmp.color = sf::Color::Green;
    tmp.font_size = 20;
    manager.add_text(tmp);

    // add ROM button
    tmp = {};
    tmp.name = "SELECT ROM";
    tmp.pos = get_graphics_pos(0,0);
    tmp.font_size = 25;
    tmp.color = sf::Color::Magenta;
    manager.add_text(tmp);

    // add reset "button"
    tmp = {};
    tmp.name = "RESET";
    tmp.pos = get_graphics_pos(80, 3);
    tmp.color = sf::Color::Green;
    tmp.font_size = 20;
    manager.add_text(tmp);

    // add program counter
    tmp = {};
    tmp.name = "PC";
    tmp.pos = get_graphics_pos(20, 60);
    manager.add_text(tmp);

    // add opcodes
    tmp = {};
    tmp.name = "Instructions";
    tmp.pos = get_graphics_pos(40, 60);
    manager.add_text(tmp);


    for (size_t i = 0; i < opcodes.size(); ++i)
    {
        tmp = P{};
        tmp.name = "OP" + std::to_string(i);
        tmp.pos = get_graphics_pos(40, i * 3, 0, 65);
        manager.add_text(tmp);
    }

    // add args
    for (size_t i = 0; i < opcodes.size(); ++i)
    {
        tmp = P{};
        tmp.name = "Args" + std::to_string(i);
        tmp.pos = get_graphics_pos(48, i * 3, 0, 65);
        manager.add_text(tmp);
    }

    // add registers
    for (int i = 0; i < 16; ++i)
    {
        P tmp;
        tmp.name = "V" + std::to_string(i);
        tmp.pos = get_graphics_pos(80, 3 * i, 0, 10);
        manager.add_text(tmp);
    }

    tmp = P{};
    tmp.name = "I";
    tmp.pos = get_graphics_pos(80, 48, 0, 15);
    manager.add_text(tmp);

    tmp = P{};
    tmp.name = "ST";
    tmp.pos = get_graphics_pos(80, 51, 0, 15);
    manager.add_text(tmp);

    tmp = P{};
    tmp.name = "DT";
    tmp.pos = get_graphics_pos(80, 54, 0, 15);
    manager.add_text(tmp);

    // add background
    tmp = P{};
    tmp.name = "background";
    tmp.dim = sf::Vector2f(window.getSize().x, window.getSize().y);
    tmp.color = sf::Color::Black;
    manager.add_rectangle(tmp);
}
    
sf::Vector2f Emulator::get_graphics_pos(int x_percent, int y_percent,
int x_offset, int y_offset) const
{
    auto [x,y] = window.getSize();
    sf::Vector2f tmp;
    tmp.x = (x / 100 * x_percent) + (x / 100 * x_offset);
    tmp.y = (y / 100 * y_percent) + (y / 100 * y_offset);

    return tmp;
}

void Emulator::reset_states()
{
    display = {};
    cpu = CPU(&ram, &keyboard, &display);
    mouse_pressed=false;
}

void Emulator::handle_userinput()
{
    if (mouse_pressed)
    {
        auto mouse_pos = sf::Mouse::getPosition(window);

        auto& slider_button = manager.get_rectangle_ref("clockspeed_slider2");
        const auto& button_b = slider_button.getGlobalBounds();
        const auto& slider_b = manager.get_rectangle_cref("clockspeed_slider1").getGlobalBounds();
        // Slider is used
        if (button_b.contains(mouse_pos.x, mouse_pos.y))
        {
            if (mouse_pos.x > slider_b.left && mouse_pos.x < slider_b.left + slider_b.width) 
            {
                cs.changing_clockspeed = true;
                slider_button.setPosition(mouse_pos.x, slider_button.getPosition().y);
                float p = (slider_button.getPosition().x - slider_b.left) / slider_b.width;
                update_clockspeed(p*100);
            }
        }
        // Halt button is pressed
        else if (manager.get_text_cref("HALT").getGlobalBounds()
                .contains(mouse_pos.x, mouse_pos.y)) 
        {
            if (cs.halt)
            {
                cs.halt = false;
                cs.clockcycles_done = cs.clockspeed / 4;
            }
            else
            {
                cs.halt = true;
            }
            mouse_pressed=false;
        }
        // Reset button is pressed
        else if (manager.get_text_cref("RESET").getGlobalBounds()
                .contains(mouse_pos.x, mouse_pos.y))
        {
            reset_states();
        }
        // Step button is pressed
        // enable stepping only when the emulation is halting
        else if (cs.halt && manager.get_text_cref("STEP").getGlobalBounds()
                .contains(mouse_pos.x, mouse_pos.y))
        {
            cs.step = true;
            mouse_pressed = false;
        }
        // SELECT ROM button is pressed
        else if(manager.get_text_cref("SELECT ROM").getGlobalBounds()
                .contains(mouse_pos.x, mouse_pos.y))
        {
            if (auto g = game_selector.get(); !g.empty())
                inject_rom(g);
            mouse_pressed = false;
        }
    }
    else
        cs.changing_clockspeed = false;
}
   
void Emulator::handle_events()
{
    sf::Event evt;
    while (window.pollEvent(evt))
    {
        switch (evt.type)
        {
            case sf::Event::Closed: 
            {
                window.close(); 
                break;
            }
            case sf::Event::KeyPressed: 
            {
                keyboard.notify(evt.key.code, true);
                break;
            }
            case sf::Event::KeyReleased:
            {
                keyboard.notify(evt.key.code, false);
                break;
            }
            case sf::Event::MouseButtonPressed:
            {
                mouse_pressed = true;
                break;
            }
            case sf::Event::MouseButtonReleased:
            {
                mouse_pressed = false;
                break;
            }
            default: break;
        }
    }
}

void Emulator::render()
{
    window.clear();

    window.draw(manager.get_rectangle_ref("background"));
    window.draw(manager.get_text_cref("PC"));
    window.draw(manager.get_text_cref("Instructions"));
    window.draw(manager.get_text_cref("I"));
    window.draw(manager.get_text_cref("DT"));
    window.draw(manager.get_text_cref("ST"));
    window.draw(manager.get_rectangle_cref("clockspeed_slider1"));
    window.draw(manager.get_rectangle_cref("clockspeed_slider2"));
    window.draw(manager.get_text_cref("clockspeed"));
    window.draw(manager.get_text_cref("RESET"));
    window.draw(manager.get_text_cref("HALT"));
    window.draw(manager.get_text_cref("STEP"));
    window.draw(manager.get_text_cref("SELECT ROM"));

    for (int i = 0; i < 16; ++i)
        window.draw(manager.get_text_cref("S" + std::to_string(i)));

    for(int i = 0; i < 16; ++i)
        window.draw(manager.get_text_cref("V" + std::to_string(i)));

    const auto& display_graphics = display.get_graphics();
    
    for (size_t i = 0; i < opcodes.size(); ++i)
        window.draw(manager.get_text_cref("OP" + std::to_string(i)));
    
    for (size_t i = 0; i < opcode_args.size(); ++i)
        window.draw(manager.get_text_cref("Args" + std::to_string(i)));

    for (auto i : display_graphics)
        window.draw(preprocess_display(i));

    window.display();
}

void Emulator::update_graphics()
{
    std::stringstream ss;

    ss << "PC: 0x" << std::hex << static_cast<int>(cpu.get_pc());
    manager.modify_string("PC", ss.str());
    ss.str("");

    if (cs.new_cycle)
    {
        opcodes[opcodes.size() - 1] = cpu.get_current_opcode();
        std::rotate(opcodes.begin(), opcodes.end() - 1, opcodes.end());

        opcode_args[opcode_args.size() - 1] = cpu.get_current_args();
        std::rotate(opcode_args.begin(), opcode_args.end() - 1, opcode_args.end());

        for (size_t i = 0; i < opcodes.size(); ++i)
            manager.modify_string("OP" + std::to_string(i), opcodes[i]);
        
        for (size_t i = 0; i < opcode_args.size(); ++i)
            manager.modify_string("Args" + std::to_string(i), opcode_args[i]);

        cs.new_cycle = false;
    }

    auto s = cpu.get_stack();
    for (int i = 0; i < 16; ++i)
    {
        if (i == cpu.get_stack_pointer())
        {
            ss << "-> Stack[" << i << "]: 0x" << std::hex 
            << s[i];
            manager.modify_string("S" + std::to_string(i), ss.str());
            ss.str("");
        }
        else
        { 
            ss << "Stack[" << i << "]: 0x" << std::hex 
            << s[i];
            manager.modify_string("S" + std::to_string(i), ss.str());
            ss.str("");
        }
    }

    for (int i = 0; i < 16; ++i)
    {
        ss << "V" << i << ": 0x" << static_cast<int>(cpu.get_reg_at(i));
        manager.modify_string("V" + std::to_string(i), ss.str());
        ss.str("");
    }

    ss << "V_I:\t 0x" << std::hex << cpu.get_I();
    manager.modify_string("I", ss.str());
    ss.str("");

    ss << "V_DT: 0x" << std::hex << static_cast<int>(cpu.get_DT());
    manager.modify_string("DT", ss.str());
    ss.str("");

    ss << "V_ST: 0x" << std::hex << static_cast<int>(cpu.get_ST());
    manager.modify_string("ST", ss.str());
    ss.str("");

    if (cs.changing_clockspeed)
    {
        if (cs.clockspeed >= 1'000'000)
            ss << "Clock: " << std::dec << cs.clockspeed / 1'000'000.0 << "mhz";
        else if (cs.clockspeed >= 1000)
            ss << "Clock: " << std::dec << cs.clockspeed / 1000.0 << "khz";
        else
            ss << "Clock: " << std::dec << cs.clockspeed << "hz";
        manager.modify_string("clockspeed", ss.str());
    }
    else
    {
        if (cs.actual_clockspeed >= 1'000'000)
            ss << "Clock: " << std::dec << cs.actual_clockspeed / 1'000'000.0 << "mhz";
        else if (cs.actual_clockspeed >= 1000)
            ss << "Clock: " << std::dec << cs.actual_clockspeed / 1000.0 << "khz";
        else
            ss << "Clock: " << std::dec << cs.actual_clockspeed << "hz";
        manager.modify_string("clockspeed", ss.str());

        // change the colour to red if bottleneck is
        // detected
        if (cs.bottleneck)
            manager.get_text_ref("clockspeed").setFillColor(sf::Color::Red);
        else
            manager.get_text_ref("clockspeed").setFillColor(sf::Color::Green);
    }
    ss.str("");

    // change colours of the halt and step button
    // depending on the user input
    if (cs.halt)
    {
        manager.get_text_ref("HALT").setFillColor(sf::Color::Red);
        manager.get_text_ref("STEP").setFillColor(sf::Color::Green);
    }
    else
    {
        manager.get_text_ref("HALT").setFillColor(sf::Color::Green);
        manager.get_text_ref("STEP").setFillColor(sf::Color::Red);
    }
}

sf::RectangleShape& Emulator::preprocess_display(sf::RectangleShape& obj)
{
    constexpr float scale = 10.F;
    auto s = obj.getSize();
    auto p = obj.getPosition();

    auto offset = get_graphics_pos(scale, scale);
    obj.setSize(sf::Vector2f(s.x * scale, s.y * scale));
    obj.setPosition(sf::Vector2f(p.x * scale + offset.x, p.y * scale + offset.y));
    return obj;
}

void Emulator::update_clockspeed(int percentage)
{
    if (percentage < 5) cs.clockspeed = 1;
    else if (percentage < 10) cs.clockspeed = 5;
    else if (percentage < 15) cs.clockspeed = 10;
    else if (percentage < 20) cs.clockspeed = 25;
    else if (percentage < 25) cs.clockspeed = 50;
    else if (percentage < 30) cs.clockspeed = 100;
    else if (percentage < 35) cs.clockspeed = 250;
    else if (percentage < 40) cs.clockspeed = 500;
    else if (percentage < 45) cs.clockspeed = 750;
    else if (percentage < 50) cs.clockspeed = 1000;
    else if (percentage < 55) cs.clockspeed = 2500;
    else if (percentage < 60) cs.clockspeed = 5000;
    else if (percentage < 70) cs.clockspeed = 10'000;
    else if (percentage < 75) cs.clockspeed = 25'000;
    else if (percentage < 80) cs.clockspeed = 50'000;
    else if (percentage < 85) cs.clockspeed = 100'000;
    else if (percentage < 95) cs.clockspeed = 1'000'000;
}

void Emulator::emulate_cpu()
{
    // return immediately if the cpu 
    // is currently halting
    if (cs.halt)
    {
        // normalize cpu_state values do 
        // prevent clockspeed fluctuation
        // upon leaving the halt state
        cs.clockcycles_done = cs.clockspeed / 4;
        cs.actual_clockspeed = cs.clockspeed;

        if (!cs.step)
            return;
        
        cs.step=false;
    }

    // adjust the interval to make the
    // clock_speed actually be independent
    // of delay due to frame rendering
    if (timer.is_ready(Timer::Type::milliseconds, 250))
    {
        if (cs.clockspeed >= 100 && cs.clockcycles_done)
        {
            cs.cycle_adjustment *= (cs.clockspeed / 4.0) / cs.clockcycles_done;
            if (cs.cycle_adjustment > 500)
            { 
                cs.cycle_adjustment = 500;
                cs.bottleneck = true;
            }
            else
                cs.bottleneck = false;

            cs.actual_clockspeed = cs.clockcycles_done * 4;
        }
        cs.clockcycles_done = 0;
    }
    if (cs.clockspeed < 100)
    {
        if (cpu_ready(cs.clockspeed))
        {
            cpu.execute_next_cycle();
            ++cs.clockcycles_done;
            cs.new_cycle = true;
            cs.actual_clockspeed = cs.clockspeed;
        }
    }
    else if (cpu_ready(cs.cycle_adjustment*cs.clockspeed))
    {
        cpu.execute_next_cycle();
        ++cs.clockcycles_done;
        cs.new_cycle = true;
    }
}

bool Emulator::cpu_ready(double hz)
{
    if (timer.is_ready(Timer::Type::cpu, hz))
        return true;
    return false;
}
   
std::vector<uint8_t> Emulator::get_bytes(const std::string& path)
{
    std::vector<uint8_t> bytes;
    std::ifstream in;
    in.open(path, std::ios::binary);
    if (!in)
        throw std::runtime_error{"Can't find " + path + "."};

    char temp;
    for (in.get(temp); !in.eof(); in.get(temp))
    {
        bytes.emplace_back(static_cast<uint8_t>(temp));
    }
    in.close();
    return bytes;
}