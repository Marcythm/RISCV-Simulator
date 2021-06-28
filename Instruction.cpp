#include "Instruction.hpp"

auto Instruction::Decode(const u32 encoding, const u32 pc, const u32 reg[32]) -> std::unique_ptr<Instruction> {
    switch (getbits<6, 0>(encoding)) {
        case 0b0010011:

        case 0b0110111:
            return std::make_unique<LUI>(encoding, pc, reg);
        case 0b0010111:

        case 0b0110011:

        case 0b1101111:

        case 0b1100111:

        case 0b1100011:

        case 0b0000011:

        case 0b0100011:
        ;
    }
    return nullptr;
}
