#pragma once

#include "config.hpp"

namespace InstTag {
    #define GENTAG_R(mnemonic, OPCODE, FUNCT3, FUNCT7)  \
        struct mnemonic {                               \
            static constexpr u32 opcode = OPCODE;       \
            static constexpr u32 funct3 = FUNCT3;       \
            static constexpr u32 funct7 = FUNCT7;       \
        };

    #define GENTAG_I(mnemonic, OPCODE, FUNCT3)          \
        struct mnemonic {                               \
            static constexpr u32 opcode = OPCODE;       \
            static constexpr u32 funct3 = FUNCT3;       \
        };
    #define GENTAG_S(mnemonic, OPCODE, FUNCT3)          \
        struct mnemonic {                               \
            static constexpr u32 opcode = OPCODE;       \
            static constexpr u32 funct3 = FUNCT3;       \
        };
    #define GENTAG_B(mnemonic, OPCODE, FUNCT3)          \
        struct mnemonic {                               \
            static constexpr u32 opcode = OPCODE;       \
            static constexpr u32 funct3 = FUNCT3;       \
        };

    #define GENTAG_U(mnemonic, OPCODE)                  \
        struct mnemonic {                               \
            static constexpr u32 opcode = OPCODE;       \
        };
    #define GENTAG_J(mnemonic, OPCODE)                  \
        struct mnemonic {                               \
            static constexpr u32 opcode = OPCODE;       \
        };

    // 2.4 Integer Computational Instructions

    // 2.4.1 Integer Register-Immediate Instructions

    GENTAG_I(ADDI,  0b0010011, 0b000);
    GENTAG_I(SLTI,  0b0010011, 0b010);
    GENTAG_I(SLTIU, 0b0010011, 0b011);

    GENTAG_I(ANDI, 0b0010011, 0b111);
    GENTAG_I(ORI,  0b0010011, 0b110);
    GENTAG_I(XORI, 0b0010011, 0b100);

    GENTAG_I(SLLI, 0b0010011, 0b001/*, 0b0000000*/);
    GENTAG_I(SRLI, 0b0010011, 0b101/*, 0b0000000*/);
    GENTAG_I(SRAI, 0b0010011, 0b101/*, 0b0100000*/);

    GENTAG_U(LUI,   0b0110111);
    GENTAG_U(AUIPC, 0b0010111);

    // 2.4.2 Integer Register-Register Operations

    GENTAG_R(ADD,  0b0110011, 0b000, 0b0000000);
    GENTAG_R(SLT,  0b0110011, 0b010, 0b0000000);
    GENTAG_R(SLTU, 0b0110011, 0b011, 0b0000000);

    GENTAG_R(AND, 0b0110011, 0b111, 0b0000000);
    GENTAG_R(OR,  0b0110011, 0b110, 0b0000000);
    GENTAG_R(XOR, 0b0110011, 0b100, 0b0000000);

    GENTAG_R(SLL, 0b0110011, 0b001, 0b0000000);
    GENTAG_R(SRL, 0b0110011, 0b101, 0b0000000);

    GENTAG_R(SUB, 0b0110011, 0b000, 0b0100000);
    GENTAG_R(SRA, 0b0110011, 0b101, 0b0100000);

    // 2.5 Control Transfer Instructions

    // 2.5.1 Unconditional Jumps

    GENTAG_J(JAL, 0b1101111);

    GENTAG_I(JALR, 0b1100111, 0b000);

    // 2.5.2 Conditional Branches

    GENTAG_B(BEQ,  0b1100011, 0b000);
    GENTAG_B(BNE,  0b1100011, 0b001);
    GENTAG_B(BLT,  0b1100011, 0b100);
    GENTAG_B(BLTU, 0b1100011, 0b110);
    GENTAG_B(BGE,  0b1100011, 0b101);
    GENTAG_B(BGEU, 0b1100011, 0b111);

    // 2.6 Load and Store Instructions

    GENTAG_I(LB,  0b0000011, 0b000);
    GENTAG_I(LH,  0b0000011, 0b001);
    GENTAG_I(LW,  0b0000011, 0b010);
    GENTAG_I(LBU, 0b0000011, 0b100);
    GENTAG_I(LHU, 0b0000011, 0b101);

    GENTAG_S(SB, 0b0100011, 0b000);
    GENTAG_S(SH, 0b0100011, 0b001);
    GENTAG_S(SW, 0b0100011, 0b010);

    #undef GENTAG
    #undef GENTAG3
    #undef GENTAG_R
}
