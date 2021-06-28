#pragma once

#include "config.hpp"

namespace EncodingTable {
    #define GENTABLE_R(mnemonic, OPCODE, FUNCT3, FUNCT7)    \
        namespace mnemonic {                                \
            constexpr u32 opcode = OPCODE;                  \
            constexpr u32 funct3 = FUNCT3;                  \
            constexpr u32 funct7 = FUNCT7;                  \
        }

    #define GENTABLE_I(mnemonic, OPCODE, FUNCT3)    \
        namespace mnemonic {                        \
            constexpr u32 opcode = OPCODE;          \
            constexpr u32 funct3 = FUNCT3;          \
        }
    #define GENTABLE_S(mnemonic, OPCODE, FUNCT3)    \
        namespace mnemonic {                        \
            constexpr u32 opcode = OPCODE;          \
            constexpr u32 funct3 = FUNCT3;          \
        }
    #define GENTABLE_B(mnemonic, OPCODE, FUNCT3)    \
        namespace mnemonic {                        \
            constexpr u32 opcode = OPCODE;          \
            constexpr u32 funct3 = FUNCT3;          \
        }

    #define GENTABLE_U(mnemonic, OPCODE)    \
        namespace mnemonic {                \
            constexpr u32 opcode = OPCODE;  \
        }
    #define GENTABLE_J(mnemonic, OPCODE)    \
        namespace mnemonic {                \
            constexpr u32 opcode = OPCODE;  \
        }

    // 2.4 Integer Computational Instructions

    // 2.4.1 Integer Register-Immediate Instructions

    GENTABLE_I(ADDI,  0b0010011, 0b000);
    GENTABLE_I(SLTI,  0b0010011, 0b010);
    GENTABLE_I(SLTIU, 0b0010011, 0b011);

    GENTABLE_I(ANDI, 0b0010011, 0b111);
    GENTABLE_I(ORI,  0b0010011, 0b110);
    GENTABLE_I(XORI, 0b0010011, 0b100);

    GENTABLE_I(SLLI, 0b0010011, 0b001/*, 0b0000000*/);
    GENTABLE_I(SRLI, 0b0010011, 0b101/*, 0b0000000*/);
    GENTABLE_I(SRAI, 0b0010011, 0b101/*, 0b0100000*/);

    GENTABLE_U(LUI,   0b0110111);
    GENTABLE_U(AUIPC, 0b0010111);

    // 2.4.2 Integer Register-Register Operations

    GENTABLE_R(ADD,  0b0110011, 0b000, 0b0000000);
    GENTABLE_R(SLT,  0b0110011, 0b010, 0b0000000);
    GENTABLE_R(SLTU, 0b0110011, 0b011, 0b0000000);

    GENTABLE_R(AND, 0b0110011, 0b111, 0b0000000);
    GENTABLE_R(OR,  0b0110011, 0b110, 0b0000000);
    GENTABLE_R(XOR, 0b0110011, 0b100, 0b0000000);

    GENTABLE_R(SLL, 0b0110011, 0b001, 0b0000000);
    GENTABLE_R(SRL, 0b0110011, 0b101, 0b0000000);

    GENTABLE_R(SUB, 0b0110011, 0b000, 0b0100000);
    GENTABLE_R(SRA, 0b0110011, 0b101, 0b0100000);

    // 2.5 Control Transfer Instructions

    // 2.5.1 Unconditional Jumps

    GENTABLE_J(JAL, 0b1101111);

    GENTABLE_I(JALR, 0b1100111, 0b000);

    // 2.5.2 Conditional Branches

    GENTABLE_B(BEQ,  0b1100011, 0b000);
    GENTABLE_B(BNE,  0b1100011, 0b001);
    GENTABLE_B(BLT,  0b1100011, 0b100);
    GENTABLE_B(BLTU, 0b1100011, 0b110);
    GENTABLE_B(BGE,  0b1100011, 0b101);
    GENTABLE_B(BGEU, 0b1100011, 0b111);

    // 2.6 Load and Store Instructions

    GENTABLE_I(LB,  0b0000011, 0b000);
    GENTABLE_I(LH,  0b0000011, 0b001);
    GENTABLE_I(LW,  0b0000011, 0b010);
    GENTABLE_I(LBU, 0b0000011, 0b100);
    GENTABLE_I(LHU, 0b0000011, 0b101);

    GENTABLE_S(SB, 0b0100011, 0b000);
    GENTABLE_S(SH, 0b0100011, 0b001);
    GENTABLE_S(SW, 0b0100011, 0b010);

    #undef GENTABLE
    #undef GENTABLE3
    #undef GENTABLE_R
}
