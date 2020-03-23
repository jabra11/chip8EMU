#ifndef CPU_HPP
#define CPU_HPP

#include <array>

#include "Registers.hpp"

class Memory;

class CPU
{
public:

    explicit CPU(Memory* ram);

    void execute_next_cycle();

    // in public for testing
    void parse_instruction(uint16_t test_opcode);

private:

    void jump_sys(const uint16_t address);
    void clear_display() const;
    void return_from_routine();
    void jump(const uint16_t address);
    void call_subroutine(const uint16_t address);
    void skip_if(const uint8_t Vx, const uint8_t byte);
    void skip_if_not(const uint8_t Vx, const uint8_t byte);
    void skip_if_r(const uint8_t Vx, const uint8_t Vy);
    void load(const uint8_t Vx, const uint8_t byte);
    void load_r(const uint8_t Vx, const uint8_t Vy);
    void add(const uint8_t Vx, const uint8_t byte);
    void OR(const uint8_t Vx, const uint8_t Vy);
    void AND(const uint8_t Vx, const uint8_t Vy);
    void XOR(const uint8_t Vx, const uint8_t Vy);
    void add_c(const uint8_t Vx, const uint8_t Vy);
    void sub(const uint8_t Vx, const uint8_t Vy);
    void shr(const uint8_t Vx);
    // TODO: finish this up (17/36)

    // store the currently executing address
    uint16_t program_counter;

    // point to the topmost level of the stack
    // startvalue of -1 to avoid call_subroutine
    // to waste a stackposition in it's first usage
    uint8_t stack_pointer = -1;

    // stores addresses that the interpreter should return
    // to after finishing a subroutine
    std::array<uint16_t, 16> stack{};

    Registers reg;
    Memory* ram = nullptr;
};

#endif // CPU_HPP