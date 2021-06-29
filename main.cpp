#include "Instruction.hpp"

auto main() -> i32 {
    u32 a, reg[32];
    ADDI inst(1, a, reg);
    inst.EX();
    return 0;
}
