#include <vector>
#include <fstream>
#include <iostream>
#include <chrono> // sleep_for helper
#include <thread> // sleep_for
#include <sstream>

#include <cstddef>
//#include <Timings.hpp>


#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Emulator.hpp"
#include "utility/Timer.hpp"
#include "utility/Object_manager.hpp"

Emulator::Emulator(int w, int h)
    :window{sf::VideoMode(w, h), "Chip8"}
{
    setup_graphics();
    init_interpreter_space();
    sb.loadFromFile("../resources/sounds/sound2.wav");
    sound.setBuffer(sb);
    sound.setVolume(0.f);
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
    tmp.pos = get_graphics_pos(50, 5);
    manager.add_rectangle(tmp);

    // add clockspeed_slider_p2
    tmp = {};
    tmp.name = "clockspeed_slider2";
    tmp.dim = get_graphics_pos(2, 4);
    tmp.pos = get_graphics_pos(50, 5);
    tmp.color = sf::Color::Red;
    manager.add_rectangle(tmp);

    // add clockspeed
    tmp = {};
    tmp.name = "clockspeed";
    tmp.pos = get_graphics_pos(20, 3);
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

void Emulator::handle_userinput()
{
    if (mouse_pressed)
    {
        auto mouse_pos = sf::Mouse::getPosition(window);
        auto& slider_button = manager.get_rectangle_ref("clockspeed_slider2");
        const auto& button_b = slider_button.getGlobalBounds();
        const auto& slider_b = manager.get_rectangle_cref("clockspeed_slider1").getGlobalBounds();

        if (button_b.contains(mouse_pos.x, mouse_pos.y))
        {
            if (mouse_pos.x > slider_b.left && mouse_pos.x < slider_b.left + slider_b.width) 
            {
                slider_button.setPosition(mouse_pos.x, slider_button.getPosition().y);
                float p = (slider_button.getPosition().x - slider_b.left) / slider_b.width;
                update_clockspeed(p*100);
            }
        }
    }
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
    
    for (int i = 0; i < 16; ++i)
        window.draw(manager.get_text_cref("S" + std::to_string(i)));

    for(int i = 0; i < 16; ++i)
        window.draw(manager.get_text_cref("V" + std::to_string(i)));

    const auto& display_graphics = display.get_graphics();
    
    for (size_t i = 0; i < opcodes.size(); ++i)
        window.draw(manager.get_text_cref("OP" + std::to_string(i)));

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

    if (new_cycle)
    {
        opcodes[opcodes.size() - 1] = cpu.get_current_opcode();
        std::rotate(opcodes.begin(), opcodes.end() - 1, opcodes.end());

        for (size_t i = 0; i < opcodes.size(); ++i)
            manager.modify_string("OP" + std::to_string(i), opcodes[i]);
        new_cycle = false;
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

    ss << "Clock: " << std::dec << clock_speed << "hz";
    manager.modify_string("clockspeed", ss.str());
    ss.str("");
}

void Emulator::buzz()
{
    sound.play();
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
    //std::cout << percentage << '\n';
    if (percentage < 5) clock_speed = 1;
    else if (percentage < 10) clock_speed = 5;
    else if (percentage < 20) clock_speed = 25;
    else if (percentage < 30) clock_speed = 100;
    else if (percentage < 40) clock_speed = 500;
    else if (percentage < 50) clock_speed = 1000;
    else if (percentage < 60) clock_speed = 5000;
    else if (percentage < 70) clock_speed = 10'000;
    else if (percentage < 80) clock_speed = 50'000;
    else if (percentage < 85) clock_speed = 100'000;
    else if (percentage < 95) clock_speed = 1'000'000;
}

void Emulator::emulate_cpu()
{
    // adjust the interval to make the
    // clock_speed actually be independent
    // of delay due to frame rendering

    if (timer.is_ready(Timer::Type::seconds, 1))
    {
        if (clock_cycles_done)
        {
            interval = static_cast<double>(clock_speed) / clock_cycles_done;
            if (!interval)
                interval = 1;
            std::cout << "cycles: " << clock_cycles_done << '\n';
            std::cout << interval << '\n';
            clock_cycles_done = 0;
        }
    }
    if (cpu_ready(clock_speed))
    {
        cpu.execute_next_cycle();
        ++clock_cycles_done;
        new_cycle = true;
    }
}

bool Emulator::cpu_ready(int interval)
{
    if (timer.is_ready(Timer::Type::cpu, interval))
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