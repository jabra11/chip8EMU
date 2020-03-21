#include <iostream>

#include "CPU.hpp"
#include "Instructions.hpp"

CPU::CPU(Memory& ram)
    :ram{ram}
{
}

void CPU::execute_next_cycle()
{
    // work in progress 
}

void CPU::parse_instruction(uint16_t test_opcode)
{ 
    // opcodes are based on 
    // http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#3.1


    // the addresses are 8bits long and store 8bit
    // values but the opcodes are 12 bits, therefore
    // it is probably necessary to load two addresses
    // and "combine" them to obtain a valid opcode
    uint16_t opcode = ram.read(program_counter);

    // testing
    opcode = test_opcode;
    if (OP::is_clear_display(opcode))               
        clear_display(); 

    else if (OP::is_return_from_routine(opcode))    
        return_from_routine();

    else if (OP::is_jump(opcode))                   
        jump(opcode & 0x0FFF);

    else if (OP::is_call_subroutine(opcode))        
        call_subroutine(opcode & 0x0FFF);

    else if (OP::is_skip_if(opcode))                
        skip_if((opcode >> 8) & 0x0F, opcode & 0x00FF);

    else if (OP::is_skip_if_not(opcode))            
        skip_if_not((opcode >> 8) & 0x0F, opcode & 0x00FF);

    else if (OP::is_skip_if_not_r(opcode))          
        skip_if_r((opcode >> 8) & 0x0F, (opcode >> 4) & 0x000F);

    else if (OP::is_load(opcode))                   
        load((opcode >> 8) & 0x0F, opcode & 0x00FF);

    else if (OP::is_add(opcode))                    
        add((opcode >> 8) & 0x0F, opcode & 0x00FF);

    else if (OP::is_load_r(opcode))                 
        load_r((opcode >> 8) & 0x0F, (opcode >> 4) & 0x000F);

    else if (OP::is_OR(opcode))                     
        OR((opcode >> 8) & 0x0F, (opcode >> 4) & 0x000F);

    else if (OP::is_AND(opcode))                    
        AND((opcode >> 8) & 0x0F, (opcode >> 4) & 0x000F);

    else if (OP::is_XOR(opcode))                    
        XOR((opcode >> 8) & 0x0F, (opcode >> 4) & 0x000F);

    else if (OP::is_add_c(opcode))                  
        add_c((opcode >> 8) & 0x0F, (opcode >> 4) & 0x000F);

    else if (OP::is_sub(opcode)) 
        sub((opcode >> 8) & 0x0F, (opcode >> 4) & 0x000F);

    else if(OP::is_shr(opcode))
        shr((opcode >> 8) & 0x0F);

    // this should be the last
    else if (OP::is_jump_sys(opcode))               
        jump_sys(opcode & 0x0FFF);
    
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

void CPU::add_c(const uint8_t Vx, const uint8_t Vy)
{
    // set V[0x0F] = 1 if result is > 8bits 
    if (reg.get(Vx) + reg.get(Vy) > 0xFF)
        reg.put(0x0F, 1);
    reg.put(Vx, reg.get(Vx) + reg.get(Vy));
}

void CPU::sub(const uint8_t Vx, const uint8_t Vy)
{
    // set V[0x0F] = 1 if Vx > Vy, otherwise 0
    if (reg.get(Vx) > reg.get(Vy))
        reg.put(0x0F, 1);
    else 
        reg.put(0x0F, 0);

    reg.put(Vx, reg.get(Vx) - reg.get(Vy));
}

void CPU::shr(const uint8_t Vx)
{
    // if the least-signif. bit of Vx is 1, set 
    // VF to 1, otherwise to 0
    if ((reg.get(Vx) & 0x01) == 0x01)
        reg.put(0x0F, 1);
    else
        reg.put(0x0F, 0);

    reg.put(Vx, reg.get(Vx) >> 2);
}

