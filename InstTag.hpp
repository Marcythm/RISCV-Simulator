#pragma once

#include "config.hpp"

namespace InstTag {
    #define GENTAG_R(MNEMONIC, OPCODESTR, OPCODE, FUNCT3, FUNCT7)   \
        struct MNEMONIC {                                           \
            static constexpr char const *opcodestr = OPCODESTR;     \
            static constexpr u32 opcode = OPCODE;                   \
            static constexpr u32 funct3 = FUNCT3;                   \
            static constexpr u32 funct7 = FUNCT7;                   \
        };

    #define GENTAG_I(MNEMONIC, OPCODESTR, OPCODE, FUNCT3)           \
        struct MNEMONIC {                                           \
            static constexpr char const *opcodestr = OPCODESTR;     \
            static constexpr u32 opcode = OPCODE;                   \
            static constexpr u32 funct3 = FUNCT3;                   \
        };
    #define GENTAG_S(MNEMONIC, OPCODESTR, OPCODE, FUNCT3)           \
        struct MNEMONIC {                                           \
            static constexpr char const *opcodestr = OPCODESTR;     \
            static constexpr u32 opcode = OPCODE;                   \
            static constexpr u32 funct3 = FUNCT3;                   \
        };
    #define GENTAG_B(MNEMONIC, OPCODESTR, OPCODE, FUNCT3)           \
        struct MNEMONIC {                                           \
            static constexpr char const *opcodestr = OPCODESTR;     \
            static constexpr u32 opcode = OPCODE;                   \
            static constexpr u32 funct3 = FUNCT3;                   \
        };

    #define GENTAG_U(MNEMONIC, OPCODESTR, OPCODE)                   \
        struct MNEMONIC {                                           \
            static constexpr char const *opcodestr = OPCODESTR;     \
            static constexpr u32 opcode = OPCODE;                   \
        };
    #define GENTAG_J(MNEMONIC, OPCODESTR, OPCODE)                   \
        struct MNEMONIC {                                           \
            static constexpr char const *opcodestr = OPCODESTR;     \
            static constexpr u32 opcode = OPCODE;                   \
        };

    // 2.4 Integer Computational Instructions

    // 2.4.1 Integer Register-Immediate Instructions

    GENTAG_I(ADDI,  "addi",  0b0010011, 0b000);
    GENTAG_I(SLTI,  "slti",  0b0010011, 0b010);
    GENTAG_I(SLTIU, "sltiu", 0b0010011, 0b011);

    GENTAG_I(ANDI,  "andi", 0b0010011, 0b111);
    GENTAG_I(ORI,   "ori",  0b0010011, 0b110);
    GENTAG_I(XORI,  "xori", 0b0010011, 0b100);

    GENTAG_I(SLLI,  "slli", 0b0010011, 0b001/*, 0b0000000*/);
    GENTAG_I(SRLI,  "srli", 0b0010011, 0b101/*, 0b0000000*/);
    GENTAG_I(SRAI,  "srai", 0b0010011, 0b101/*, 0b0100000*/);

    GENTAG_U(LUI,   "lui",   0b0110111);
    GENTAG_U(AUIPC, "auipc", 0b0010111);

    // 2.4.2 Integer Register-Register Operations

    GENTAG_R(ADD,   "add",  0b0110011, 0b000, 0b0000000);
    GENTAG_R(SLT,   "slt",  0b0110011, 0b010, 0b0000000);
    GENTAG_R(SLTU,  "sltu", 0b0110011, 0b011, 0b0000000);

    GENTAG_R(AND,   "and", 0b0110011, 0b111, 0b0000000);
    GENTAG_R(OR,    "or",  0b0110011, 0b110, 0b0000000);
    GENTAG_R(XOR,   "xor", 0b0110011, 0b100, 0b0000000);

    GENTAG_R(SLL,   "sll", 0b0110011, 0b001, 0b0000000);
    GENTAG_R(SRL,   "srl", 0b0110011, 0b101, 0b0000000);

    GENTAG_R(SUB,   "sub", 0b0110011, 0b000, 0b0100000);
    GENTAG_R(SRA,   "sra", 0b0110011, 0b101, 0b0100000);

    // 2.5 Control Transfer Instructions

    // 2.5.1 Unconditional Jumps

    GENTAG_J(JAL,   "jal", 0b1101111);

    GENTAG_I(JALR,  "jalr", 0b1100111, 0b000);

    // 2.5.2 Conditional Branches

    GENTAG_B(BEQ,   "beq",  0b1100011, 0b000);
    GENTAG_B(BNE,   "bne",  0b1100011, 0b001);
    GENTAG_B(BLT,   "blt",  0b1100011, 0b100);
    GENTAG_B(BLTU,  "bltu", 0b1100011, 0b110);
    GENTAG_B(BGE,   "bge",  0b1100011, 0b101);
    GENTAG_B(BGEU,  "bgeu", 0b1100011, 0b111);

    // 2.6 Load and Store Instructions

    GENTAG_I(LB,    "lb",  0b0000011, 0b000);
    GENTAG_I(LH,    "lh",  0b0000011, 0b001);
    GENTAG_I(LW,    "lw",  0b0000011, 0b010);
    GENTAG_I(LBU,   "lbu", 0b0000011, 0b100);
    GENTAG_I(LHU,   "lhu", 0b0000011, 0b101);

    GENTAG_S(SB,    "sb", 0b0100011, 0b000);
    GENTAG_S(SH,    "sh", 0b0100011, 0b001);
    GENTAG_S(SW,    "sw", 0b0100011, 0b010);

    #undef GENTAG
    #undef GENTAG3
    #undef GENTAG_R
}
