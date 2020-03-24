#include <iostream>

#include "CPU.hpp"
#include "Instructions.hpp"
#include "../memory/Memory.hpp"

CPU::CPU(Memory* ram)
    :ram{ram}
{
}

void CPU::execute_next_cycle()
{
    parse_instruction(0xFFFF);
}

uint16_t CPU::fetch_opcode()
{
    // the addresses are 8bits long and store 8bit
    // values but the opcodes are 16 bits, therefore
    // it is necessary to load two addresses
    // and "combine" them to obtain a valid opcode
    //
    // All instructions are 2 bytes long and are stored most 
    // significant byte first. In memory the first byte of each
    // instruction should be located at an even address
    // 
    // if a program includes sprite data, it should be padded
    // to ensure the opcodes to be aligned properly

    uint16_t opcode = ram->read(program_counter);
    ++program_counter;
    opcode = opcode << 8;
    opcode += ram->read(program_counter);
    ++program_counter;

    std::cout << std::hex << opcode << std::endl;
    return opcode;
}

void CPU::parse_instruction(uint16_t test_opcode)
{ 
    // opcodes are based on 
    // http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#3.1
    uint16_t opcode = fetch_opcode();
    
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

    else if (OP::is_skip_if_rb(opcode))                
        skip_if_rb((opcode >> 8) & 0x0F, opcode & 0x00FF);

    else if (OP::is_skip_if_not_rb(opcode))            
        skip_if_not_rb((opcode >> 8) & 0x0F, opcode & 0x00FF);

    else if (OP::is_skip_if_rr(opcode))          
        skip_if_rr((opcode >> 8) & 0x0F, (opcode >> 4) & 0x000F);

    else if (OP::is_load_rb(opcode))                   
        load_rb((opcode >> 8) & 0x0F, opcode & 0x00FF);

    else if (OP::is_add(opcode))                    
        add((opcode >> 8) & 0x0F, opcode & 0x00FF);

    else if (OP::is_load_rr(opcode))                 
        load_rr((opcode >> 8) & 0x0F, (opcode >> 4) & 0x000F);

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

    else if(OP::is_sub_n(opcode))
        sub_n((opcode >> 8) & 0x0F, (opcode >> 4) & 0x000F);

    else if(OP::is_shl(opcode))
        shl((opcode >> 8) & 0x0F);

    else if(OP::is_skip_if_not_rr(opcode))
        skip_if_not_rr((opcode >> 8) & 0x0F, (opcode >> 4) & 0x000F);

    else if(OP::is_load_ai(opcode))
        load_ai(opcode & 0x0FFF);

    else if(OP::is_jump_ra(opcode))
        jump_ra(opcode & 0x0FFF);
    
    else if(OP::is_rnd(opcode))
        rnd((opcode >> 8) & 0x0F, opcode & 0x00FF);

    // this should be the last
    else if (OP::is_jump_sys(opcode))               
        jump_sys(opcode & 0x0FFF);
    
    
}

void CPU::jump_sys(const uint16_t address)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    program_counter = address;
}

void CPU::clear_display() const
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    // work in progress
}

void CPU::return_from_routine()
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;

    if (stack_pointer >= 0)
        program_counter = stack[stack_pointer--];
    else
        std::cerr << "ERROR: Stack pointer has an invalid value\n";
}

void CPU::jump(const uint16_t address)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;

    program_counter = address;
}

void CPU::call_subroutine(const uint16_t address)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;

    stack[++stack_pointer] = program_counter;
    program_counter = address;
}

void CPU::skip_if_rb(const uint8_t Vx, const uint8_t byte)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    if (reg.get(Vx) == byte)
        program_counter += 2;
}

void CPU::skip_if_not_rb(const uint8_t Vx, const uint8_t byte)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    if (reg.get(Vx) != byte)
        program_counter += 2;
}

void CPU::skip_if_rr(const uint8_t Vx, const uint8_t Vy)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    if (reg.get(Vx) == reg.get(Vy))
        program_counter += 2;
}
    
void CPU::load_rb(const uint8_t Vx, const uint8_t byte)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    reg.put(Vx, byte);
}

void CPU::load_rr(const uint8_t Vx, const uint8_t Vy)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    reg.put(Vx, reg.get(Vy));
}

void CPU::add(const uint8_t Vx, const uint8_t byte)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    reg.put(Vx, reg.get(Vx) + byte);
}

void CPU::OR(const uint8_t Vx, const uint8_t Vy)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    reg.put(Vx, (reg.get(Vx) | reg.get(Vy)));
}

void CPU::AND(const uint8_t Vx, const uint8_t Vy)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    reg.put(Vx, (reg.get(Vx) & reg.get(Vy)));
}
    
void CPU::XOR(const uint8_t Vx, const uint8_t Vy)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    reg.put(Vx, (reg.get(Vx) ^ reg.get(Vy)));
}

void CPU::add_c(const uint8_t Vx, const uint8_t Vy)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    // set V[0x0F] = 1 if result is > 8bits 
    if (reg.get(Vx) + reg.get(Vy) > 0xFF)
        reg.put(0x0F, 1);
    reg.put(Vx, reg.get(Vx) + reg.get(Vy));
}

void CPU::sub(const uint8_t Vx, const uint8_t Vy)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    // set V[0x0F] = 1 if Vx > Vy, otherwise 0
    if (reg.get(Vx) > reg.get(Vy))
        reg.put(0x0F, 1);
    else 
        reg.put(0x0F, 0);

    reg.put(Vx, reg.get(Vx) - reg.get(Vy));
}

void CPU::shr(const uint8_t Vx)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    // if the least-signif. bit of Vx is 1, set 
    // VF to 1, otherwise to 0
    if ((reg.get(Vx) & 0x01) == 0x01)
        reg.put(0x0F, 1);
    else
        reg.put(0x0F, 0);

    reg.put(Vx, reg.get(Vx) >> 2);
}

void CPU::sub_n(const uint8_t Vx, const uint8_t Vy)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    // set V[0x0F] = 1 if Vy > Vx, otherwise 0
    if (reg.get(Vx) > reg.get(Vy))
        reg.put(0x0F, 1);
    else 
        reg.put(0x0F, 0);

    reg.put(Vx, reg.get(Vy) - reg.get(Vx));
}

void CPU::shl(const uint8_t Vx)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    if (reg.get(Vx) >> 3 == 0x01)
        reg.put(0x0F, 1);
    else
        reg.put(0x0F, 0);
     
    reg.put(Vx, reg.get(Vx) << 2);
}

void CPU::skip_if_not_rr(const uint8_t Vx, const uint8_t Vy)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    if (reg.get(Vx) != reg.get(Vy))
        program_counter += 2;
}

void CPU::load_ai(const uint16_t address)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    reg.I = address;
}

void CPU::jump_ra(const uint16_t address)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;

    program_counter = address + reg.get(0x0);
}

void CPU::rnd(const uint8_t Vx, const uint8_t byte)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
    
    reg.put(Vx, rng.get(0,255) & byte);
}