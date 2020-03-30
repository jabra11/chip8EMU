#include <vector>
#include <fstream>
#include <iostream>
#include <chrono> // sleep_for helper
#include <thread> // sleep_for
#include <sstream>

#include <cstddef>


#include <SFML/Graphics.hpp>

#include "Emulator.hpp"
#include "cpu/Timer.hpp"
#include "Object_manager.hpp"

Emulator::Emulator(int w, int h)
    :window{sf::VideoMode(w, h), "Chip8EMU"}
{
    using P = Object_manager::Object_property;

    // add program counter
    P tmp;
    tmp.name = "PC";
    tmp.pos = sf::Vector2f(w/2, h/2);
    tmp.color = sf::Color::Green;
    manager.add_text(tmp);

    // add opcode
    tmp = P{};
    tmp.name = "Instruction";
    tmp.color = sf::Color::Green;
    tmp.pos = sf::Vector2f(w / 3, h/10 * 1);
    manager.add_text(tmp);

    // add registers
    for (int i = 0; i < 16; ++i)
    {
        P tmp;
        tmp.name = "V" + std::to_string(i);
        tmp.color = sf::Color::Green;
        tmp.pos = sf::Vector2f(w / 10 * 7, h / 20 * i + 50);

        manager.add_text(tmp);
    }

    // add background
    tmp = P{};
    tmp.name = "background";
    tmp.dim = sf::Vector2f(w, h);

    manager.add_rectangle(tmp);
}

void Emulator::run()
{
    std::chrono::steady_clock tim;
    auto point = tim.now();
    while (window.isOpen())
    {
        handle_events();
        emulate_cpu();

        if(timer.is_ready(Timer::Type::sound, 144))
        {
            update_graphics();
            render();
        }
        if (std::chrono::duration_cast<std::chrono::milliseconds>
            (tim.now() - point).count() > 1000)
        {
            std::cout << clock_cycles_done << std::endl;
            clock_cycles_done = 0;
            point = tim.now();
        }
    }
}

void Emulator::inject_rom(const std::string &path, int start_index)
{
    auto bytes = get_bytes(path);

    for (size_t i = 0; i < bytes.size(); ++i) 
        ram.write(start_index + i, bytes[i]);
}
   
void Emulator::handle_events()
{
    sf::Event evt;
    while (window.pollEvent(evt))
    {
        switch (evt.type)
        {
            case sf::Event::Closed: 
                window.close(); 
                break;
            case sf::Event::KeyPressed: 
                keyboard.notify(evt.key.code, true);
                break;
            case sf::Event::KeyReleased:
                keyboard.notify(evt.key.code, false);
                break;
            default: break;
        }
    }
}

void Emulator::render()
{
    window.clear();

    window.draw(manager.get_rectangle_ref("background"));
    window.draw(manager.get_text_cref("PC"));
    window.draw(manager.get_text_cref("Instruction"));
    
    for(int i = 0; i < 16; ++i)
        window.draw(manager.get_text_cref("V" + std::to_string(i)));

    window.display();
}

void Emulator::update_graphics()
{
    std::stringstream ss;

    ss << "PC: 0x" << std::hex << static_cast<int>(cpu.get_pc());
    manager.modify_string("PC", ss.str());
    ss.str("");

    ss << "OP: " << cpu.get_current_opcode();
    manager.modify_string("Instruction", ss.str());
    ss.str("");

    for (int i = 0; i < 9; ++i)
    {
        ss << "V" << i << ": 0x" << static_cast<int>(cpu.get_reg_at(i));
        manager.modify_string("V" + std::to_string(i), ss.str());
        ss.str("");
    }

    for (int i = 9; i < 16; ++i)
    {
        ss << "V" << i << ": 0x" << static_cast<int>(cpu.get_reg_at(i));
        manager.modify_string("V" + std::to_string(i), ss.str());
        ss.str("");
    }

    //manager.get_text_ref("PC").setString("PC: " 
    //        + std::to_string(cpu.get_pc()));
    
    //manager.get_text_ref("Instruction").setString("Current opcode: " + 
    //        cpu.get_current_opcode());
    //for (int i = 0; i < 9; ++i)
    //{
    //    manager.get_text_ref("V" + std::to_string(i)).setString("V0"
    //        + std::to_string(i) + ": " + std::to_string(cpu.get_reg_at(i)));
    //}
    //for (int i = 9; i < 16; ++i)
    //{
    //    manager.get_text_ref("V" + std::to_string(i)).setString("V"
    //        + std::to_string(i) + ": " + std::to_string(cpu.get_reg_at(i)));
    //}
}

void Emulator::emulate_cpu()
{
    if (cpu_ready())
    {
        cpu.execute_next_cycle();
        ++clock_cycles_done;
    }
}

bool Emulator::cpu_ready()
{
    if (timer.is_ready(Timer::Type::delay, clock_speed))
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