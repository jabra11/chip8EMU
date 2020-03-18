#include <array>

class CPU
{
public:
private:

    // store the currently executing address
    uint16_t program_counter;

    // point to the topmost level of the stack
    uint8_t stack_pointer;

    // stores addresses that the interpreter should return
    // to after finishing a subroutine
    std::array<uint16_t, 16> stack;
};