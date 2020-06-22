#include <iostream>
#include <exception>
#include <iomanip>

#include "CPU.hpp"
#include "Instructions.hpp"
#include "../memory/Memory.hpp"
#include "../keyboard/Keyboard.hpp"
#include "../display/Display.hpp"

CPU::CPU(Memory* ram, Keyboard* kb, Display* dp)
    :ram{ram}, keyboard{kb}, display{dp}
{
}

void CPU::execute_next_cycle()
{
    parse_instruction();
    update_timers();
}

uint16_t CPU::get_I() const
{
    return reg.I;
}

uint8_t CPU::get_ST() const
{
    return reg.ST;
}

uint8_t CPU::get_DT() const
{
    return reg.DT;
}

uint16_t CPU::get_pc() const
{
    return program_counter;
}

uint8_t CPU::get_reg_at(uint8_t index) const
{
    return reg.get(index);
}

std::string CPU::get_current_opcode()
{
    return opcode.str();
}
    
std::string CPU::get_current_args()
{
    return args.str();
}

uint8_t CPU::get_stack_pointer() const
{
    return stack_pointer;
}

const std::array<uint16_t, 16>& CPU::get_stack() const
{
    return stack;
}

void CPU::update_timers()
{
    if (reg.DT)
    {
        if (timer.is_ready(CPU_timer::Type::delay, 60))
            --reg.DT;
    }
    if (reg.ST)
    {
        if (timer.is_ready(CPU_timer::Type::sound, 60))
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
        throw std::runtime_error{"program counter missaligned"};

    uint16_t opcode = ram->read(program_counter++);
    opcode = opcode << 8;
    opcode += ram->read(program_counter++);

    return opcode;
}


void CPU::parse_instruction()
{ 
    // clear streams
    opcode.str("");
    args.str("");

    // opcodes are based on 
    // http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#3.1
    uint16_t opcode = fetch_opcode();

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
        opcode << "SYS";
        
    program_counter = address;
}

void CPU::clear_display()
{
    if constexpr(debug)
        opcode << "CLS";
        
    display->clear_display();
}

void CPU::return_from_routine()
{
    if constexpr(debug)
        opcode << "RET";

    program_counter = stack[stack_pointer];
    stack[stack_pointer--] = 0;
}

void CPU::jump(uint16_t address)
{
    if constexpr(debug)
    {
        opcode << "JP";
        args << "0x" << std::hex << address;
    }

    program_counter = address;
}

void CPU::call_subroutine(uint16_t address)
{
    if constexpr(debug)
    {
        opcode << "CALL";
        args << "0x" << std::hex << address;
    }

    stack[++stack_pointer] = program_counter;
    program_counter = address;
}

void CPU::skip_if_rb(uint8_t Vx, uint8_t byte)
{
    if constexpr(debug)
    {
        opcode << "SE";
        args << "V" << std::hex 
                << static_cast<int>(Vx) << ", 0x" << static_cast<int>(byte);
    }
        
    if (reg.get(Vx) == byte)
        program_counter += 2;
}

void CPU::skip_if_not_rb(uint8_t Vx, uint8_t byte)
{
    if constexpr(debug)
    {
        opcode << "SNE";
        args << "V" << std::hex 
                << static_cast<int>(Vx) << ", 0x" << static_cast<int>(byte);
    }
        
    if (reg.get(Vx) != byte)
        program_counter += 2;
}

void CPU::skip_if_rr(uint8_t Vx, uint8_t Vy)
{
    if constexpr(debug)
    {
        opcode << "SE";
        args << "V" << std::hex 
                << static_cast<int>(Vx) << ", V" << static_cast<int>(Vy);
    }    
    if (reg.get(Vx) == reg.get(Vy))
        program_counter += 2;
}
    
void CPU::load_rb(uint8_t Vx, uint8_t byte)
{
    if constexpr(debug)
    {
        opcode << "LD";
        args << "V" << std::hex 
                << static_cast<int>(Vx) << ", 0x" << static_cast<int>(byte);
    }
    reg.put(Vx, byte);
}

void CPU::load_rr(uint8_t Vx, uint8_t Vy)
{
    if constexpr(debug)
    {
        opcode << "LD"; 
        args << "V" << std::hex 
                << static_cast<int>(Vx) << ", V" << static_cast<int>(Vy);
    }
    reg.put(Vx, reg.get(Vy));
}

void CPU::add(uint8_t Vx, uint8_t byte)
{
    if constexpr(debug)
    {
        opcode << "ADD"; 
        args << "V" << std::hex 
                << static_cast<int>(Vx) << ", 0x" << static_cast<int>(byte);
    }    
    reg.put(Vx, reg.get(Vx) + byte);
}

void CPU::OR(uint8_t Vx, uint8_t Vy)
{
    if constexpr(debug)
    {
        opcode << "OR"; 
        args << "V" << std::hex 
                << static_cast<int>(Vx) << ", V" << static_cast<int>(Vy);
    }   
    reg.put(Vx, (reg.get(Vx) | reg.get(Vy)));
}

void CPU::AND(uint8_t Vx, uint8_t Vy)
{
    if constexpr(debug)
    {
        opcode << "AND"; 
        args << "V" << std::hex
                << static_cast<int>(Vx) << ", V" << static_cast<int>(Vy);
    }   
    reg.put(Vx, (reg.get(Vx) & reg.get(Vy)));
}
    
void CPU::XOR(uint8_t Vx, uint8_t Vy)
{
    if constexpr(debug)
    {
        opcode << "XOR"; 
        args << "V" << std::hex 
                << static_cast<int>(Vx) << ", V" << static_cast<int>(Vy);
    }   
    reg.put(Vx, (reg.get(Vx) ^ reg.get(Vy)));
}

void CPU::add_c(uint8_t Vx, uint8_t Vy)
{
    if constexpr(debug)
    {
        opcode << "ADD"; 
        args << "V" << std::hex 
                << static_cast<int>(Vx) << ", V" << static_cast<int>(Vy);
    }   

    // set V[0x0F] = 1 if result is > 8bits 
    if (reg.get(Vx) + reg.get(Vy) > 0xFF)
        reg.put(0x0F, 1);
    reg.put(Vx, reg.get(Vx) + reg.get(Vy));
}

void CPU::sub(uint8_t Vx, uint8_t Vy)
{
    if constexpr(debug)
    {
        opcode << "SUB"; 
        args << "V" << std::hex 
                << static_cast<int>(Vx) << ", V" << static_cast<int>(Vy);
    }

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
    {
        opcode << "SHR"; 
        args << "V" << std::hex << static_cast<int>(Vx);
    }   

    // if the least-signif. bit of Vx is 1, set 
    // VF to 1, otherwise to 0
    if ((reg.get(Vx) & 0x01) == 0x01)
        reg.put(0x0F, 1);
    else
        reg.put(0x0F, 0);

    reg.put(Vx, reg.get(Vx) >> 1);
}

void CPU::sub_n(uint8_t Vx, uint8_t Vy)
{
    if constexpr(debug)
    {
        opcode << "SUBN"; 
        args << "V" << std::hex 
                << static_cast<int>(Vx) << ", V" << static_cast<int>(Vy);
    }   

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
    {
        opcode << "SHL"; 
        args << "V" << std::hex << static_cast<int>(Vx);
    }   
    if (reg.get(Vx) >> 3 == 0x01)
        reg.put(0x0F, 1);
    else
        reg.put(0x0F, 0);
     
    reg.put(Vx, reg.get(Vx) << 1);
}

void CPU::skip_if_not_rr(uint8_t Vx, uint8_t Vy)
{
    if constexpr(debug)
    {
        opcode << "SNE"; 
        args << "V" << std::hex 
                << static_cast<int>(Vx) << ", V" << static_cast<int>(Vy);
    }   
    if (reg.get(Vx) != reg.get(Vy))
        program_counter += 2;
}

void CPU::load_ai(uint16_t address)
{
    if constexpr(debug)
    {
        opcode << "LD"; 
        args << "I, " << std::hex 
                << address;
    }   
    reg.I = address;
}

void CPU::jump_ra(uint16_t address)
{
    if constexpr(debug)
    {
        opcode << "JP"; 
        args << "V0 " << std::hex 
                << address;
    }
    program_counter = address + reg.get(0x0);
}

void CPU::rnd(uint8_t Vx, uint8_t byte)
{
    if constexpr(debug) 
    {
        opcode << "RND"; 
        args << "V" << std::hex 
                << static_cast<int>(Vx) << ", 0x" << static_cast<int>(byte);
    } 
    reg.put(Vx, rng.get(0,255) & byte);
}

void CPU::draw(uint8_t Vx, uint8_t Vy, uint8_t amount_sprites)
{
    if constexpr(debug) 
    {
        opcode << "DRW"; 
        args << "V" << std::hex 
                << static_cast<int>(Vx) << ", V" << static_cast<int>(Vy) << ", " << static_cast<int>(amount_sprites);
    }

    bool collision = false;
    for (int i = 0; i < amount_sprites; ++i)
    {
        // set VF to 1 if pixel collision happened
        if (display->add_graphic(reg.get(Vx), reg.get(Vy) + i, ram->read(reg.I + i)))
            collision = true;
    }
    if (collision)
        reg.put(0xF, 1);
    else
        reg.put(0xF, 0);
}

void CPU::skip_if_pressed(uint8_t Vx)
{
    if constexpr(debug) 
    {
        opcode << "SKP"; 
        args << "V" << std::hex 
                << static_cast<int>(Vx);
    }   
    if (keyboard->check(reg.get(Vx)))
        program_counter += 2;
}

void CPU::skip_if_not_pressed(uint8_t Vx)
{
    if constexpr(debug) 
    {
        opcode << "SKNP"; 
        args << "V" << std::hex 
                << static_cast<int>(Vx);
    }
    if (!keyboard->check(reg.get(Vx)))
        program_counter += 2;
}

void CPU::load_delay(uint8_t Vx)
{
    if constexpr(debug) 
    {
        opcode << "LD"; 
        args << "V" << std::hex 
                << static_cast<int>(Vx) << ", DT";
    }

    reg.put(Vx, reg.DT);
}

void CPU::load_key(uint8_t Vx)
{
    if constexpr(debug) 
    {
        opcode << "LD"; 
        args << "V" << std::hex 
                << static_cast<int>(Vx) << ", K";
    }

    for (int i = 0; i < 16; ++i)
    {
        if (keyboard->check(i))
        {
            reg.put(Vx, i);
            return; 
        }
    }

    // reset the PC to this instruction
    // to basically hold all execution
    // as described in the TS
    program_counter -= 2;
}

void CPU::set_delay(uint8_t Vx)
{
    if constexpr(debug) 
    {
        opcode << "LD"; 
        args << "DT ," << std::hex 
                << "V" << static_cast<int>(Vx);
    }

    reg.DT = reg.get(Vx);
}

void CPU::set_sound(uint8_t Vx)
{
    if constexpr(debug) 
    {
        opcode << "LD"; 
        args << "ST ," << std::hex 
                << "V" << static_cast<int>(Vx);
    }

    reg.ST = reg.get(Vx);
}

void CPU::add_ir(uint8_t Vx)
{
    if constexpr(debug) 
    {
        opcode << "ADD"; 
        args << "I ," << "V"
                << std::hex << static_cast<int>(Vx);
    }

    reg.I += reg.get(Vx);
}

void CPU::load_sprite_loc(uint8_t Vx)
{
    if constexpr(debug) 
    {
        opcode << "LD"; 
        args << "F " << std::hex 
                << ",V" << static_cast<int>(Vx);
    }

    // startaddress defined in 
    // void Emulator::init_interpreter_space()
    constexpr int start_address = 0x100;
    reg.I = (reg.get(Vx) * 5) + start_address;
}

void CPU::load_BCD(uint8_t Vx)
{
    if constexpr(debug) 
    {
        opcode << "LD"; 
        args << "B " << std::hex 
                << ",V" << static_cast<int>(Vx);
    }
    auto tmp = reg.get(Vx);
    ram->write(reg.I, tmp / 100);
    ram->write(reg.I + 0x01, (tmp / 10) % 10);
    ram->write(reg.I + 0x02, tmp % 10);
}

void CPU::dump_reg(uint8_t Vx)
{
    if constexpr(debug) 
    {
        opcode << "LD"; 
        args << "I, " << std::hex 
                << "V" << static_cast<int>(Vx);
    }
    
    // not sure if it is supposed to copy from
    // V0 to Vx or from V0 to reg.read(Vx)
    // this might become an error
    for (int i = 0; i <= Vx; ++i)
        ram->write(reg.I + i, reg.get(i));
}

void CPU::load_reg(uint8_t Vx)
{
    if constexpr(debug) 
    {
        opcode << "LD"; args << std::hex 
                << "V" << static_cast<int>(Vx) << ", I";
    }
    
    // not sure if it is supposed to copy from
    // V0 to Vx or from V0 to reg.read(Vx)
    // this might become an error
    for (int i = 0; i <= Vx; ++i)
        reg.put(i, ram->read(reg.I + i));
}
