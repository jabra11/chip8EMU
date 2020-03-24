// http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#3.1


// nnn or addr - A 12-bit value, the lowest 12 bits of the instruction
// n or nibble - A 4-bit value, the lowest 4 bits of the instruction
// x - A 4-bit value, the lower 4 bits of the high byte of the instruction
// y - A 4-bit value, the upper 4 bits of the low byte of the instruction
// kk or byte - An 8-bit value, the lowest 8 bits of the instruction

#ifndef INSTRUCTIONS_HPP
#define INSTRUCTIONS_HPP

#include <cstdint>
#include <iostream>

class OP
{
public:
    OP() = delete;

    // 0nnn - SYS addr
    
    static constexpr bool is_jump_sys(const uint16_t opcode)
    {
        return opcode >> 12 == 0x0;
    }

    // 00E0 - CLS
    static constexpr bool is_clear_display(const uint16_t opcode)
    {
        return opcode == 0x00E0;
    }

    // 00EE - RET
    static constexpr bool is_return_from_routine(const uint16_t opcode)
    {
        return opcode == 0x00EE;
    }

    // 1nnn - JP addr
    static constexpr bool is_jump(const uint16_t opcode)
    {
        return opcode >> 12 == 0x01;
    }

    // 2nnn - CALL addr
    static constexpr bool is_call_subroutine(const uint16_t opcode)
    {
        return opcode >> 12 == 0x02;
    }

    // 3xkk - SE Vx, byte
    static constexpr bool is_skip_if_rb(const uint16_t opcode)
    {
        return opcode >> 12 == 0x03;
    }

    // 4xkk - SNE Vx, byte
    static constexpr bool is_skip_if_not_rb(const uint16_t opcode)
    {
        return opcode >> 12 == 0x04;
    }

    // 5xy0 - SE Vx, Vy
    static constexpr bool is_skip_if_rr(const uint16_t opcode)
    {
        return opcode >> 12 == 0x05 && (opcode & 0x00F) == 0x0; 
    }
    
    // 6xkk - LD Vx, byte
    static constexpr bool is_load_rb(const uint16_t opcode)
    {
        return opcode >> 12 == 0x06;
    }
    
    // 7xkk - ADD Vx, byte
    static constexpr bool is_add(const uint16_t opcode)
    {
        return opcode >> 12 == 0x07;
    }

    // 8xy0 - LD Vx, Vy
    static constexpr bool is_load_rr(const uint16_t opcode)
    {
        return opcode >> 12 == 0x08 && (opcode & 0x00F) == 0x0;
    }

    // 8xy1 - OR Vx, Vy
    static constexpr bool is_OR(const uint16_t opcode)
    {
        return opcode >> 12 == 0x08 && (opcode & 0x00F) == 0x1;
    }

    // 8xy2 - AND Vx, Vy
    static constexpr bool is_AND(const uint16_t opcode)
    {
        return opcode >> 12 == 0x08 && (opcode & 0x0F) == 0x2;
    }

    // 8xy3 - XOR Vx, Vy
    static constexpr bool is_XOR(const uint16_t opcode)
    {
        return (opcode >> 12 == 0x08) && (opcode & 0x0F) == 0x3;
    }

    // 8xy4 - ADD Vx, Vy (with carry)
    static constexpr bool is_add_c(const uint16_t opcode)
    {
        return opcode >> 12 == 0x08 && (opcode & 0x0F) == 0x4;
    }

    // 8xy5 - SUB Vx, Vy
    static constexpr bool is_sub(const uint16_t opcode)
    {
        return opcode >> 12 == 0x08 && (opcode & 0x0F) == 0x5;
    }

    // 8xy6 - SHR Vx {, Vy}
    static constexpr bool is_shr(const uint16_t opcode)
    {
        return opcode >> 12 == 0x08 && (opcode & 0x0F) == 0x6;
    }

    // 8xy7 - SUBN Vx, Vy
    static constexpr bool is_sub_n(const uint16_t opcode)
    {
        return opcode >> 12 == 0x08 && (opcode & 0x0F) == 0x7;
    }

    // 8xyE - SHL Vx
    static constexpr bool is_shl(const uint16_t opcode)
    {
        return opcode >> 12 == 0x08 && (opcode & 0x0F) == 0xE;
    }

    // 9xy0 - SNE Vx, Vy
    static constexpr bool is_skip_if_not_rr(const uint16_t opcode)
    {
        return opcode >> 12 == 0x09 && (opcode & 0x0F) == 0x0;
    }

    // Annn - LD I, addr
    static constexpr bool is_load_ai(const uint16_t opcode)
    {
        return opcode >> 12 == 0x0A;
    }

    // Bnnn - JP V0 - addr
    static constexpr bool is_jump_ra(const uint16_t opcode)
    {
        return opcode >> 12 == 0x0B;
    }

    static constexpr bool is_rnd(const uint16_t opcode)
    {
        return opcode >> 12 == 0x0C;
    }
    
};

#endif // INSTRUCTIONS_HPP