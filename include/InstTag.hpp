#pragma once

#include "config.hpp"

namespace InstTag {
  #define GENTAG(TAGNAME, OPCODE)                                 \
    struct TAGNAME {                                            \
      static constexpr u32 opcode = OPCODE;                   \
    };

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

  #define GENOPCODE(NAME, OPCODE) constexpr u32 OPC_##NAME = OPCODE

  GENOPCODE(LOAD,     0b0000011);
  GENOPCODE(OP_IMM,   0b0010011);
  GENOPCODE(AUIPC,    0b0010111);
  GENOPCODE(STORE,    0b0100011);
  GENOPCODE(OP,       0b0110011);
  GENOPCODE(LUI,      0b0110111);
  GENOPCODE(BRANCH,   0b1100011);
  GENOPCODE(JALR,     0b1100111);
  GENOPCODE(JAL,      0b1101111);

  // 2.4 Integer Computational Instructions

  // 2.4.1 Integer Register-Immediate Instructions
  GENTAG  (ALU_ri,         OPC_OP_IMM);
  GENTAG_I(ADDI,  "addi",  OPC_OP_IMM, 0b000);
  GENTAG_I(SLTI,  "slti",  OPC_OP_IMM, 0b010);
  GENTAG_I(SLTIU, "sltiu", OPC_OP_IMM, 0b011);
  GENTAG_I(ANDI,  "andi",  OPC_OP_IMM, 0b111);
  GENTAG_I(ORI,   "ori",   OPC_OP_IMM, 0b110);
  GENTAG_I(XORI,  "xori",  OPC_OP_IMM, 0b100);

  GENTAG  (Shift_ri,       OPC_OP_IMM);
  GENTAG_I(SLLI,  "slli",  OPC_OP_IMM, 0b001/*, 0b0000000*/);
  GENTAG_I(SRLI,  "srli",  OPC_OP_IMM, 0b101/*, 0b0000000*/);
  GENTAG_I(SRAI,  "srai",  OPC_OP_IMM, 0b101/*, 0b0100000*/);

  GENTAG_U(LUI,   "lui",   OPC_LUI);
  GENTAG_U(AUIPC, "auipc", OPC_AUIPC);

  // 2.4.2 Integer Register-Register Operations
  GENTAG  (ALU_rr,         OPC_OP);
  GENTAG_R(ADD,   "add",   OPC_OP, 0b000, 0b0000000);
  GENTAG_R(SUB,   "sub",   OPC_OP, 0b000, 0b0100000);
  GENTAG_R(SLT,   "slt",   OPC_OP, 0b010, 0b0000000);
  GENTAG_R(SLTU,  "sltu",  OPC_OP, 0b011, 0b0000000);
  GENTAG_R(AND,   "and",   OPC_OP, 0b111, 0b0000000);
  GENTAG_R(OR,    "or",    OPC_OP, 0b110, 0b0000000);
  GENTAG_R(XOR,   "xor",   OPC_OP, 0b100, 0b0000000);
  GENTAG_R(SLL,   "sll",   OPC_OP, 0b001, 0b0000000);
  GENTAG_R(SRL,   "srl",   OPC_OP, 0b101, 0b0000000);
  GENTAG_R(SRA,   "sra",   OPC_OP, 0b101, 0b0100000);

  // 2.5 Control Transfer Instructions

  // 2.5.1 Unconditional Jumps
  GENTAG_J(JAL,   "jal",   OPC_JAL);
  GENTAG_I(JALR,  "jalr",  OPC_JALR, 0b000);

  // 2.5.2 conditional Branches
  GENTAG  (BranchCC_rri,   OPC_BRANCH);
  GENTAG_B(BEQ,   "beq",   OPC_BRANCH, 0b000);
  GENTAG_B(BNE,   "bne",   OPC_BRANCH, 0b001);
  GENTAG_B(BLT,   "blt",   OPC_BRANCH, 0b100);
  GENTAG_B(BLTU,  "bltu",  OPC_BRANCH, 0b110);
  GENTAG_B(BGE,   "bge",   OPC_BRANCH, 0b101);
  GENTAG_B(BGEU,  "bgeu",  OPC_BRANCH, 0b111);

  // 2.6 Load and Store Instructions
  GENTAG  (Load_ri,        OPC_LOAD);
  GENTAG_I(LB,    "lb",    OPC_LOAD, 0b000);
  GENTAG_I(LH,    "lh",    OPC_LOAD, 0b001);
  GENTAG_I(LW,    "lw",    OPC_LOAD, 0b010);
  GENTAG_I(LBU,   "lbu",   OPC_LOAD, 0b100);
  GENTAG_I(LHU,   "lhu",   OPC_LOAD, 0b101);

  GENTAG  (Store_rri,      OPC_STORE);
  GENTAG_S(SB,    "sb",    OPC_STORE, 0b000);
  GENTAG_S(SH,    "sh",    OPC_STORE, 0b001);
  GENTAG_S(SW,    "sw",    OPC_STORE, 0b010);

  // GENTAG(Unknown, 0b0000000);
  struct Unknown {
    static constexpr u32 opcode = 0b0000000;
    static constexpr char const *opcodestr = "unknown";
  };

  #undef GENTAG
  #undef GENTAG_R
  #undef GENTAG_I
  #undef GENTAG_S
  #undef GENTAG_B
  #undef GENTAG_U
  #undef GENTAG_J
}
