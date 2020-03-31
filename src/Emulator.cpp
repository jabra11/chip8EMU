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
    setup_graphics();
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

void Emulator::setup_graphics()
{
    using P = Object_manager::Object_property;

    // add call stack
    for (int i = 0; i < 8; ++i)
    {
        P tmp;
        tmp.name = "S" + std::to_string(i);
        tmp.pos = set_graphics_pos(20, i * 3, 0, 65);
        manager.add_text(tmp);
    }

    for (int i = 7; i < 16; ++i)
    {
        P tmp;
        tmp.name = "S" + std::to_string(i);
        tmp.pos = set_graphics_pos(40, (i - 8) * 3, 0, 65);
        manager.add_text(tmp);
    }

    // add program counter
    P tmp;
    tmp.name = "PC";
    tmp.pos = set_graphics_pos(20, 60);
    manager.add_text(tmp);

    // add opcode
    tmp = P{};
    tmp.name = "Instruction";
    tmp.pos = set_graphics_pos(40, 60);
    manager.add_text(tmp);

    // add registers
    for (int i = 0; i < 16; ++i)
    {
        P tmp;
        tmp.name = "V" + std::to_string(i);
        tmp.pos = set_graphics_pos(70, 3 * i, 0, 10);
        manager.add_text(tmp);
    }

    tmp = P{};
    tmp.name = "I";
    tmp.pos = set_graphics_pos(70, 48, 0, 15);
    manager.add_text(tmp);

    tmp = P{};
    tmp.name = "ST";
    tmp.pos = set_graphics_pos(70, 51, 0, 15);
    manager.add_text(tmp);

    tmp = P{};
    tmp.name = "DT";
    tmp.pos = set_graphics_pos(70, 54, 0, 15);
    manager.add_text(tmp);

    // add background
    tmp = P{};
    tmp.name = "background";
    tmp.dim = sf::Vector2f(window.getSize().x, window.getSize().y);
    tmp.color = sf::Color::Black;
    manager.add_rectangle(tmp);
}
    
sf::Vector2f Emulator::set_graphics_pos(int x_percent, int y_percent,
int x_offset, int y_offset) const
{
    auto [x,y] = window.getSize();
    sf::Vector2f tmp;
    tmp.x = (x / 100 * x_percent) + (x / 100 * x_offset);
    tmp.y = (y / 100 * y_percent) + (y / 100 * y_offset);

    return tmp;
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
    window.draw(manager.get_text_cref("I"));
    window.draw(manager.get_text_cref("DT"));
    window.draw(manager.get_text_cref("ST"));
    
    for (int i = 0; i < 16; ++i)
        window.draw(manager.get_text_cref("S" + std::to_string(i)));

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

    //for (int i = 9; i < 16; ++i)
    //{
    //    ss << "V" << i << ": 0x" << static_cast<int>(cpu.get_reg_at(i));
    //    manager.modify_string("V" + std::to_string(i), ss.str());
    //    ss.str("");
    //}

    ss << "V_I:\t 0x" << std::hex << cpu.get_I();
    manager.modify_string("I", ss.str());
    ss.str("");

    ss << "V_DT: 0x" << std::hex << static_cast<int>(cpu.get_DT());
    manager.modify_string("DT", ss.str());
    ss.str("");

    ss << "V_ST: 0x" << std::hex << static_cast<int>(cpu.get_ST());
    manager.modify_string("ST", ss.str());
    ss.str("");

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