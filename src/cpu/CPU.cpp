#include <iostream>

#include "CPU.hpp"
#include "Instructions.hpp"
#include "../memory/Memory.hpp"

CPU::CPU(Memory* ram, Keyboard* kb, Display* dp)
    :ram{ram}, keyboard{kb}, display{dp}
{
}

void CPU::execute_next_cycle()
{
    parse_instruction(0xFFFF);
    update_timers();
}

void CPU::update_timers()
{
    if (reg.DT)
    {
        if (timer.is_ready(Timer::Type::delay, 60))
            --reg.DT;
    }
    if (reg.ST)
    {
        if (timer.is_ready(Timer::Type::sound, 60))
            --reg.ST;
    }
}

uint16_t CPU::fetch_opcode()
{
    // the addresses are 12bits long and store 8bit
    // values but the opcodes are 16 bits long, therefore
    // it is necessary to load two addresses
    // and "combine" them to obtain a valid opcode
    //
    // All instructions are 2 bytes long and are stored most 
    // significant byte first. In memory the first byte of each
    // instruction should be located at an even address
    // 
    // if a program includes sprite data, it should be padded
    // to ensure the opcodes to be aligned properly
    if (program_counter % 2 != 0)
        throw std::exception{"program counter missaligned"};

    uint16_t opcode = ram->read(program_counter++);
    opcode = opcode << 8;
    opcode += ram->read(program_counter++);

    // testing
    //std::cout << std::hex << opcode << std::endl;
    return opcode;
}

void CPU::parse_instruction(uint16_t test_opcode)
{ 
    // opcodes are based on 
    // http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#3.1
    uint16_t opcode = fetch_opcode();
    
    // testing
    //opcode = test_opcode;

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

    else if(OP::is_draw(opcode))
        draw((opcode >> 8) & 0x0F, (opcode >> 4) & 0x0F, opcode & 0x0F);
    
    else if(OP::is_skip_if_pressed(opcode))
        skip_if_pressed((opcode >> 8) & 0x0F);

    else if(OP::is_skip_if_not_pressed(opcode))
        skip_if_not_pressed((opcode >> 8) & 0x0F);

    else if(OP::is_load_delay(opcode)) 
        load_delay((opcode >> 8) & 0x0F);
    
    else if(OP::is_load_key(opcode))
        load_key((opcode >> 8) & 0x0F);

    else if(OP::is_set_delay(opcode))
        set_delay((opcode >> 8) & 0x0F);
    
    else if(OP::is_set_sound(opcode))
        set_sound((opcode >> 8) & 0x0F);

    else if(OP::is_add_ir(opcode))
        add_ir((opcode >> 8) & 0x0F);

    else if(OP::is_load_sprite_loc(opcode))
        load_sprite_loc((opcode >> 8) & 0x0F);

    else if(OP::is_load_BCD(opcode))
        load_BCD((opcode >> 8) & 0x0F);

    else if(OP::is_dump_reg(opcode))
        dump_reg((opcode >> 8) & 0x0F);
        
    else if(OP::is_load_reg(opcode))
        load_reg((opcode >> 8) & 0x0F);

    // this should be right before the else branch
    else if(OP::is_jump_sys(opcode))               
        jump_sys(opcode & 0x0FFF);
    else
        throw std::runtime_error{"Unsupported instruction."};
    
}

void CPU::jump_sys(uint16_t address)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    program_counter = address;
}

void CPU::clear_display()
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    // work in progress
}

void CPU::return_from_routine()
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;

    program_counter = stack[stack_pointer--];
}

void CPU::jump(uint16_t address)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;

    program_counter = address;
}

void CPU::call_subroutine(uint16_t address)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;

    stack[++stack_pointer] = program_counter;
    program_counter = address;
}

void CPU::skip_if_rb(uint8_t Vx, uint8_t byte)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    if (reg.get(Vx) == byte)
        program_counter += 2;
}

void CPU::skip_if_not_rb(uint8_t Vx, uint8_t byte)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    if (reg.get(Vx) != byte)
        program_counter += 2;
}

void CPU::skip_if_rr(uint8_t Vx, uint8_t Vy)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    if (reg.get(Vx) == reg.get(Vy))
        program_counter += 2;
}
    
void CPU::load_rb(uint8_t Vx, uint8_t byte)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    reg.put(Vx, byte);
}

void CPU::load_rr(uint8_t Vx, uint8_t Vy)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    reg.put(Vx, reg.get(Vy));
}

void CPU::add(uint8_t Vx, uint8_t byte)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    reg.put(Vx, reg.get(Vx) + byte);
}

void CPU::OR(uint8_t Vx, uint8_t Vy)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    reg.put(Vx, (reg.get(Vx) | reg.get(Vy)));
}

void CPU::AND(uint8_t Vx, uint8_t Vy)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    reg.put(Vx, (reg.get(Vx) & reg.get(Vy)));
}
    
void CPU::XOR(uint8_t Vx, uint8_t Vy)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    reg.put(Vx, (reg.get(Vx) ^ reg.get(Vy)));
}

void CPU::add_c(uint8_t Vx, uint8_t Vy)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    // set V[0x0F] = 1 if result is > 8bits 
    if (reg.get(Vx) + reg.get(Vy) > 0xFF)
        reg.put(0x0F, 1);
    reg.put(Vx, reg.get(Vx) + reg.get(Vy));
}

void CPU::sub(uint8_t Vx, uint8_t Vy)
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

void CPU::shr(uint8_t Vx)
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

void CPU::sub_n(uint8_t Vx, uint8_t Vy)
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

void CPU::shl(uint8_t Vx)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    if (reg.get(Vx) >> 3 == 0x01)
        reg.put(0x0F, 1);
    else
        reg.put(0x0F, 0);
     
    reg.put(Vx, reg.get(Vx) << 2);
}

void CPU::skip_if_not_rr(uint8_t Vx, uint8_t Vy)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    if (reg.get(Vx) != reg.get(Vy))
        program_counter += 2;
}

void CPU::load_ai(uint16_t address)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;
        
    reg.I = address;
}

void CPU::jump_ra(uint16_t address)
{
    if constexpr(debug)
        std::cout << __FUNCTION__ << std::endl;

    program_counter = address + reg.get(0x0);
}

void CPU::rnd(uint8_t Vx, uint8_t byte)
{
    if constexpr(debug) 
        std::cout << __FUNCTION__ << std::endl;
    
    reg.put(Vx, rng.get(0,255) & byte);
}

void CPU::draw(uint8_t Vx, uint8_t Vy, uint8_t amount_sprites)
{
    if constexpr(debug) 
        std::cout << __FUNCTION__ << std::endl;
    
    // work in progress
}

void CPU::skip_if_pressed(uint8_t Vx)
{
    if constexpr(debug) 
        std::cout << __FUNCTION__ << std::endl;

    // work in progress 
}

void CPU::skip_if_not_pressed(uint8_t Vx)
{
    if constexpr(debug) 
        std::cout << __FUNCTION__ << std::endl;

    // work in progress 
}

void CPU::load_delay(uint8_t Vx)
{
    if constexpr(debug) 
        std::cout << __FUNCTION__ << std::endl;

    reg.put(Vx, reg.DT);
}

void CPU::load_key(uint8_t Vx)
{
    if constexpr(debug) 
        std::cout << __FUNCTION__ << std::endl;

    program_counter -= 2;
    // work in progress 
}

void CPU::set_delay(uint8_t Vx)
{
    if constexpr(debug) 
        std::cout << __FUNCTION__ << std::endl;

    reg.DT = reg.get(Vx);
}

void CPU::set_sound(uint8_t Vx)
{
    if constexpr(debug) 
        std::cout << __FUNCTION__ << std::endl;

    reg.ST = reg.get(Vx);
}

void CPU::add_ir(uint8_t Vx)
{
    if constexpr(debug) 
        std::cout << __FUNCTION__ << std::endl;

    reg.I += reg.get(Vx);
}

void CPU::load_sprite_loc(uint8_t Vx)
{
    if constexpr(debug) 
        std::cout << __FUNCTION__ << std::endl;

    // work in progress 
}

void CPU::load_BCD(uint8_t Vx)
{
    if constexpr(debug) 
        std::cout << __FUNCTION__ << std::endl;
    
    auto tmp = reg.get(Vx);
    ram->write(reg.I, tmp / 100);
    ram->write(reg.I + 0x01, (tmp / 10) % 10);
    ram->write(reg.I + 0x02, tmp % 10);
}

void CPU::dump_reg(uint8_t Vx)
{
    if constexpr(debug) 
        std::cout << __FUNCTION__ << std::endl;
    
    // not sure if it is supposed to copy from
    // V0 to Vx or from V0 to reg.read(Vx)
    // this might become an error
    for (int i = 0; i <= Vx; ++i)
        ram->write(reg.I + i, reg.get(i));
}

void CPU::load_reg(uint8_t Vx)
{
    if constexpr(debug) 
        std::cout << __FUNCTION__ << std::endl;
    
    // not sure if it is supposed to copy from
    // V0 to Vx or from V0 to reg.read(Vx)
    // this might become an error
    for (int i = 0; i <= Vx; ++i)
        reg.put(i, ram->read(reg.I + i));
}