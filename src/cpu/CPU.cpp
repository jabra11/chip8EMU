#include <iostream>

#include "CPU.hpp"

void CPU::execute_next_cycle()
{
    // work in progress 
}

void CPU::parse_instruction(uint16_t test_opcode)
{ 
    // opcodes are based on 
    // http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#3.1

    auto opcode = program_counter;

    // testing
    opcode = test_opcode;
    if (opcode == 0x00E0)                   clear_display(); 
    else if (opcode == 0x00EE)              return_from_routine();
    else if (opcode >> 12 == 0x01)          jump(opcode & 0x0FFF);
    else if (opcode >> 12 == 0x02)          call_subroutine(opcode & 0x0FFF);
    else if (opcode >> 12 == 0x03)          skip_if((opcode >> 8) & 0x0F, opcode & 0x00FF);
    else if (opcode >> 12 == 0x04)          skip_if_not((opcode >> 8) & 0x0F, opcode & 0x00FF);
    else if (opcode >> 12 == 0x05
            && (opcode & 0x00F) == 0x0)     skip_if_r((opcode >> 8) & 0x0F, (opcode >> 4) & 0x000F);
    else if (opcode >> 12 == 0x06)          load((opcode >> 8) & 0x0F, opcode & 0x00FF);
    else if (opcode >> 12 == 0x07)          add((opcode >> 8) & 0x0F, opcode & 0x00FF);
    else if (opcode >> 12 == 0x08
            && (opcode & 0x00f) == 0x0)     load_r((opcode >> 8) & 0x0F, (opcode >> 4) & 0x000F);
    else if (opcode >> 12 == 0x08
            && (opcode & 0x00f) == 0x1)     OR((opcode >> 8) & 0x0F, (opcode >> 4) & 0x000F);
    else if (opcode >> 12 == 0x08
            && (opcode & 0x00f) == 0x2)     AND((opcode >> 8) & 0x0F, (opcode >> 4) & 0x000F);
    else if (opcode >> 12 == 0x08
            && (opcode & 0x00f) == 0x3)     XOR((opcode >> 8) & 0x0F, (opcode >> 4) & 0x000F);
    
    // this should be the last
    else if (opcode >> 12 == 0x0)           jump_sys(opcode & 0x0FFF);
}

void CPU::jump_sys(const uint16_t address)
{
    program_counter = address;
}

void CPU::clear_display() const
{
    // work in progress
}

void CPU::return_from_routine()
{
    if (stack_pointer >= 0)
        program_counter = stack[stack_pointer--];
    else
        std::cerr << "ERROR: Stack pointer has an invalid value\n";
}

void CPU::jump(const uint16_t address)
{
    program_counter = address;
}

void CPU::call_subroutine(const uint16_t address)
{
    stack[++stack_pointer] = program_counter;
    program_counter = address;
}

void CPU::skip_if(const uint8_t Vx, const uint8_t byte)
{
    if (reg.get(Vx) == byte)
        program_counter += 2;
}

void CPU::skip_if_not(const uint8_t Vx, const uint8_t byte)
{
    if (reg.get(Vx) != byte)
        program_counter += 2;
}

void CPU::skip_if_r(const uint8_t Vx, const uint8_t Vy)
{
    if (reg.get(Vx) == reg.get(Vy))
        program_counter += 2;
}
    
void CPU::load(const uint8_t Vx, const uint8_t byte)
{
    reg.put(Vx, byte);
}

void CPU::load_r(const uint8_t Vx, const uint8_t Vy)
{
    reg.put(Vx, reg.get(Vy));
}

void CPU::add(const uint8_t Vx, const uint8_t byte)
{
    reg.put(Vx, reg.get(Vx) + byte);
}

void CPU::OR(const uint8_t Vx, const uint8_t Vy)
{
    reg.put(Vx, (reg.get(Vx) | reg.get(Vy)));
}

void CPU::AND(const uint8_t Vx, const uint8_t Vy)
{
    reg.put(Vx, (reg.get(Vx) & reg.get(Vy)));
}
    
void CPU::XOR(const uint8_t Vx, const uint8_t Vy)
{
    reg.put(Vx, (reg.get(Vx) ^ reg.get(Vy)));
}