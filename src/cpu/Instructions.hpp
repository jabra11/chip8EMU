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
    
    static constexpr bool is_jump_sys(uint16_t opcode)
    {
        return opcode >> 12 == 0x0;
    }

    // 00E0 - CLS
    static constexpr bool is_clear_display(uint16_t opcode)
    {
        return opcode == 0x00E0;
    }

    // 00EE - RET
    static constexpr bool is_return_from_routine(uint16_t opcode)
    {
        return opcode == 0x00EE;
    }

    // 1nnn - JP addr
    static constexpr bool is_jump(uint16_t opcode)
    {
        return opcode >> 12 == 0x01;
    }

    // 2nnn - CALL addr
    static constexpr bool is_call_subroutine(uint16_t opcode)
    {
        return opcode >> 12 == 0x02;
    }

    // 3xkk - SE Vx, byte
    static constexpr bool is_skip_if_rb(uint16_t opcode)
    {
        return opcode >> 12 == 0x03;
    }

    // 4xkk - SNE Vx, byte
    static constexpr bool is_skip_if_not_rb(uint16_t opcode)
    {
        return opcode >> 12 == 0x04;
    }

    // 5xy0 - SE Vx, Vy
    static constexpr bool is_skip_if_rr(uint16_t opcode)
    {
        return opcode >> 12 == 0x05 && (opcode & 0x00F) == 0x0; 
    }
    
    // 6xkk - LD Vx, byte
    static constexpr bool is_load_rb(uint16_t opcode)
    {
        return opcode >> 12 == 0x06;
    }
    
    // 7xkk - ADD Vx, byte
    static constexpr bool is_add(uint16_t opcode)
    {
        return opcode >> 12 == 0x07;
    }

    // 8xy0 - LD Vx, Vy
    static constexpr bool is_load_rr(uint16_t opcode)
    {
        return opcode >> 12 == 0x08 && (opcode & 0x00F) == 0x0;
    }

    // 8xy1 - OR Vx, Vy
    static constexpr bool is_OR(uint16_t opcode)
    {
        return opcode >> 12 == 0x08 && (opcode & 0x00F) == 0x1;
    }

    // 8xy2 - AND Vx, Vy
    static constexpr bool is_AND(uint16_t opcode)
    {
        return opcode >> 12 == 0x08 && (opcode & 0x0F) == 0x2;
    }

    // 8xy3 - XOR Vx, Vy
    static constexpr bool is_XOR(uint16_t opcode)
    {
        return (opcode >> 12 == 0x08) && (opcode & 0x0F) == 0x3;
    }

    // 8xy4 - ADD Vx, Vy (with carry)
    static constexpr bool is_add_c(uint16_t opcode)
    {
        return opcode >> 12 == 0x08 && (opcode & 0x0F) == 0x4;
    }

    // 8xy5 - SUB Vx, Vy
    static constexpr bool is_sub(uint16_t opcode)
    {
        return opcode >> 12 == 0x08 && (opcode & 0x0F) == 0x5;
    }

    // 8xy6 - SHR Vx {, Vy}
    static constexpr bool is_shr(uint16_t opcode)
    {
        return opcode >> 12 == 0x08 && (opcode & 0x0F) == 0x6;
    }

    // 8xy7 - SUBN Vx, Vy
    static constexpr bool is_sub_n(uint16_t opcode)
    {
        return opcode >> 12 == 0x08 && (opcode & 0x0F) == 0x7;
    }

    // 8xyE - SHL Vx
    static constexpr bool is_shl(uint16_t opcode)
    {
        return opcode >> 12 == 0x08 && (opcode & 0x0F) == 0xE;
    }

    // 9xy0 - SNE Vx, Vy
    static constexpr bool is_skip_if_not_rr(uint16_t opcode)
    {
        return opcode >> 12 == 0x09 && (opcode & 0x0F) == 0x0;
    }

    // Annn - LD I, addr
    static constexpr bool is_load_ai(uint16_t opcode)
    {
        return opcode >> 12 == 0x0A;
    }

    // Bnnn - JP V0 - addr
    static constexpr bool is_jump_ra(uint16_t opcode)
    {
        return opcode >> 12 == 0x0B;
    }

    // Cxkk - RND Vx, byte
    static constexpr bool is_rnd(uint16_t opcode)
    {
        return opcode >> 12 == 0x0C;
    }

    // Dxyn - DRW Vx, Vy, nibble
    static constexpr bool is_draw(uint16_t opcode)
    {
        return opcode >> 12 == 0x0D;
    }
    
    // Ex9E - SKP Vx
    static constexpr bool is_skip_if_pressed(uint16_t opcode)
    {
        return opcode >> 12 == 0x0E 
            && ((opcode >> 4) & 0x0F) == 0x09
            && (opcode & 0x0F) == 0x0E;
    }

    // ExA1 - SKNP Vx 
    static constexpr bool is_skip_if_not_pressed(uint16_t opcode)
    {
        return opcode >> 12 == 0x0E
            && ((opcode >> 4) & 0x0F) == 0x0A
            && (opcode & 0x0F) == 0x01;
    }

    // Fx07 - LD Vx, DT
    static constexpr bool is_load_delay(uint16_t opcode)
    {
        return opcode >> 12 == 0x0F 
            && (opcode & 0x0F) == 0x07;
    }

    // Fx0A - LD Vx, K
    static constexpr bool is_load_key(uint16_t opcode)
    {
        return opcode >> 12 == 0x0F
            && (opcode & 0x0F) == 0x0A;
    }

    // Fx15 - LD DT, Vx
    static constexpr bool is_set_delay(uint16_t opcode)
    {
        return opcode >> 12 == 0x0F
            && (opcode & 0xFF) == 0x15;
    }

    // Fx18 - LD ST, Vx
    static constexpr bool is_set_sound(uint16_t opcode)
    {
        return opcode >> 12 == 0x0F
            && (opcode & 0xFF) == 0x18;
    }

    // Fx1E - ADD I, Vx
    static constexpr bool is_add_ir(uint16_t opcode)
    {
        return opcode >> 12 == 0x0F
            && (opcode & 0xFF) == 0x1E;
    }

    // Fx29 - LD I(F?), Vx
    static constexpr bool is_load_sprite_loc(uint16_t opcode)
    {
        return opcode >> 12 == 0x0F
            && (opcode & 0xFF) == 0x29;
    }

    // Fx33 - LD B, Vx
    static constexpr bool is_load_BCD(uint16_t opcode)
    {
        return opcode >> 12 == 0x0F
            && (opcode & 0xFF) == 0x33;
    }

    // Fx55 - LD [I], Vx
    static constexpr bool is_dump_reg(uint16_t opcode)
    {
        return opcode >> 12 == 0x0F
            && (opcode & 0xFF) == 0x55;
    }

    // Fx65 - LD Vx, [I]
    static constexpr bool is_load_reg(uint16_t opcode)
    {
        return opcode >> 12 == 0x0F
            && (opcode & 0xFF) == 0x65;
    }
};

#endif // INSTRUCTIONS_HPP