#pragma once

#include "config.hpp"

namespace EncodingTable {
    #define GENTABLE(mnemonic, OPCODE)      \
        namespace mnemonic {                \
            constexpr u32 opcode = OPCODE;  \
        }

    #define GENTABLE3(mnemonic, OPCODE, FUNCT3) \
        namespace mnemonic {                    \
            constexpr u32 opcode = OPCODE;      \
            constexpr u32 funct3 = FUNCT3;      \
        }

    #define GENTABLE37(mnemonic, OPCODE, FUNCT3, FUNCT7)    \
        namespace mnemonic {                                \
            constexpr u32 opcode = OPCODE;                  \
            constexpr u32 funct3 = FUNCT3;                  \
            constexpr u32 funct7 = FUNCT7;                  \
        }

    // 2.4 Integer Computational Instructions

    // 2.4.1 Integer Register-Immediate Instructions

    GENTABLE3(ADDI,  0b0010011, 0b000); // I
    GENTABLE3(SLTI,  0b0010011, 0b010); // I
    GENTABLE3(SLTIU, 0b0010011, 0b011); // I

    GENTABLE3(ANDI, 0b0010011, 0b111);  // I
    GENTABLE3(ORI,  0b0010011, 0b110);  // I
    GENTABLE3(XORI, 0b0010011, 0b100);  // I

    GENTABLE37(SLLI, 0b0010011, 0b001, 0b0000000);  // I
    GENTABLE37(SRLI, 0b0010011, 0b101, 0b0000000);  // I
    GENTABLE37(SRAI, 0b0010011, 0b101, 0b0100000);  // I

    GENTABLE(LUI,   0b0110111); // U
    GENTABLE(AUIPC, 0b0010111); // U

    // 2.4.2 Integer Register-Register Operations

    GENTABLE37(ADD,  0b0110011, 0b000, 0b0000000);  // R
    GENTABLE37(SLT,  0b0110011, 0b010, 0b0000000);  // R
    GENTABLE37(SLTU, 0b0110011, 0b011, 0b0000000);  // R

    GENTABLE37(AND, 0b0110011, 0b111, 0b0000000);   // R
    GENTABLE37(OR,  0b0110011, 0b110, 0b0000000);   // R
    GENTABLE37(XOR, 0b0110011, 0b100, 0b0000000);   // R

    GENTABLE37(SLL, 0b0110011, 0b001, 0b0000000);   // R
    GENTABLE37(SRL, 0b0110011, 0b101, 0b0000000);   // R

    GENTABLE37(SUB, 0b0110011, 0b000, 0b0100000);   // R
    GENTABLE37(SRA, 0b0110011, 0b101, 0b0100000);   // R

    // 2.5 Control Transfer Instructions

    // 2.5.1 Unconditional Jumps

    GENTABLE(JAL, 0b1101111);   // J

    GENTABLE3(JALR, 0b1100111, 0b000);  // I

    // 2.5.2 Conditional Branches

    GENTABLE3(BEQ,  0b1100011, 0b000);  // B
    GENTABLE3(BNE,  0b1100011, 0b001);  // B
    GENTABLE3(BLT,  0b1100011, 0b100);  // B
    GENTABLE3(BLTU, 0b1100011, 0b110);  // B
    GENTABLE3(BGE,  0b1100011, 0b101);  // B
    GENTABLE3(BGEU, 0b1100011, 0b111);  // B

    // 2.6 Load and Store Instructions

    GENTABLE3(LB,  0b0000011, 0b000);   // I
    GENTABLE3(LH,  0b0000011, 0b001);   // I
    GENTABLE3(LW,  0b0000011, 0b010);   // I
    GENTABLE3(LBU, 0b0000011, 0b100);   // I
    GENTABLE3(LHU, 0b0000011, 0b101);   // I

    GENTABLE3(SB, 0b0100011, 0b000);    // S
    GENTABLE3(SH, 0b0100011, 0b001);    // S
    GENTABLE3(SW, 0b0100011, 0b010);    // S

    #undef GENTABLE
    #undef GENTABLE3
    #undef GENTABLE37
}
