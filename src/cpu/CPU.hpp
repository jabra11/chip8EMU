#ifndef CPU_HPP
#define CPU_HPP

#include <array>
#include <sstream>

#include "Registers.hpp"
#include "RNG.hpp"
#include "CPU_timer.hpp"

class Memory;
class Display;
class Keyboard;

class CPU
{
public:

    CPU(Memory* ram, Keyboard* kb, Display* dp);

    void execute_next_cycle();
    uint16_t get_I() const;
    uint8_t get_ST() const;
    uint8_t get_DT() const;
    uint16_t get_pc() const;
    uint8_t get_reg_at(uint8_t index) const;
    std::string get_current_opcode();
    std::string get_current_args();
    uint8_t get_stack_pointer() const;
    const std::array<uint16_t, 16>& get_stack() const;

private:
    static constexpr bool debug = true;

    uint16_t fetch_opcode();
    void update_timers();

    void parse_instruction();

    void jump_sys(uint16_t address);
    void clear_display();
    void return_from_routine();
    void jump(uint16_t address);
    void call_subroutine(uint16_t address);
    void skip_if_rb(uint8_t Vx, uint8_t byte);
    void skip_if_not_rb(uint8_t Vx, uint8_t byte);
    void skip_if_rr(uint8_t Vx, uint8_t Vy);
    void load_rb(uint8_t Vx, uint8_t byte);
    void load_rr(uint8_t Vx, uint8_t Vy);
    void add(uint8_t Vx, uint8_t byte);
    void OR(uint8_t Vx, uint8_t Vy);
    void AND(uint8_t Vx, uint8_t Vy);
    void XOR(uint8_t Vx, uint8_t Vy);
    void add_c(uint8_t Vx, uint8_t Vy);
    void sub(uint8_t Vx, uint8_t Vy);
    void shr(uint8_t Vx);
    void sub_n(uint8_t Vx, uint8_t Vy);
    void shl(uint8_t Vx);
    void skip_if_not_rr(uint8_t Vx, uint8_t Vy);
    void load_ai(uint16_t address);
    void jump_ra(uint16_t address);
    void rnd(uint8_t Vx, uint8_t byte);
    void draw(uint8_t Vx, uint8_t Vy, uint8_t amount_sprites);
    void skip_if_pressed(uint8_t Vx);
    void skip_if_not_pressed(uint8_t Vx);
    void load_delay(uint8_t Vx);
    void load_key(uint8_t Vx);
    void set_delay(uint8_t Vx);
    void set_sound(uint8_t Vx);
    void add_ir(uint8_t Vx);
    void load_sprite_loc(uint8_t Vx);
    void load_BCD(uint8_t Vx);
    void dump_reg(uint8_t Vx);
    void load_reg(uint8_t Vx);
    
    // store the currently executing address
    uint16_t program_counter = 0x200;

    std::stringstream opcode;
    std::stringstream args;

    // point to the topmost level of the stack
    // startvalue of 255 to avoid call_subroutine
    // to waste a stackposition in it's first usage
    // (it will just wrap to 0)
    uint8_t stack_pointer = 255;

    // stores addresses that the interpreter should return
    // to after finishing a subroutine
    std::array<uint16_t, 16> stack{};

    CPU_timer timer;
    RNG rng;
    Registers reg;

    Memory* ram = nullptr;
    Keyboard* keyboard = nullptr;
    Display* display = nullptr;
};

#endif // CPU_HPP