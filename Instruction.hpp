#pragma once

#include "config.hpp"
#include "EncodingTable.hpp"
#include "Memory.hpp"
#include "Utility.hpp"

struct Instruction {
    u32 encoding;
    u32 pc;

    Instruction(const u32 encoding, const u32 pc):
        encoding(encoding), pc(pc) {}

    static auto Decode(const u32 encoding, const u32 pc, const u32 reg[32])
        -> std::unique_ptr<Instruction>;

    virtual auto Execute() -> void {}
    virtual auto MemAccess() -> void {}
    virtual auto WriteBack() -> void {}
};

/* ---------- InstFormats ---------- */


template <u32 opcode_, u32 funct3_, u32 funct7_>
struct InstFormatR: Instruction {
	static constexpr u32 opcode = opcode_;
	static constexpr u32 funct3 = funct3_;
	static constexpr u32 funct7 = funct7_;

    u32 rs1, rs2, rd;
    u32 result;
    InstFormatR(const u32 encoding, const u32 pc, const u32 reg[32]):
        Instruction(encoding, pc),
        rs1(getbits<19, 15>(encoding)),
        rs2(getbits<24, 20>(encoding)),
        rd(getbits<11, 7>(encoding)) {}

    static auto Decode(const u32 encoding, const u32 pc, const u32 reg[32])
        -> std::unique_ptr<InstFormatR>;
};

#define BEG_INST_R(mnemonic)                                            \
    struct mnemonic: InstFormatR<EncodingTable::mnemonic::opcode,       \
                                 EncodingTable::mnemonic::funct3,       \
                                 EncodingTable::mnemonic::funct7> {     \
        mnemonic(const u32 encoding, const u32 pc, const u32 reg[32]):  \
            InstFormatR(encoding, pc, reg) {}
#define END_INST_R(mnemonic)                                            \
    };


template <u32 opcode_, u32 funct3_>
struct InstFormatI: Instruction {
	static constexpr u32 opcode = opcode_;
	static constexpr u32 funct3 = funct3_;

    u32 rs1, rd, imm; // imm length: 12
    u32 result;
    InstFormatI(const u32 encoding, const u32 pc, const u32 reg[32]):
        Instruction(encoding, pc),
        rs1(getbits<19, 15>(encoding)),
        rd(getbits<11, 7>(encoding)),
        imm(getbits<31, 20>(encoding)) {
        rs1 = reg[rs1];
    }

    static auto Decode(const u32 encoding, const u32 pc, const u32 reg[32])
        -> std::unique_ptr<InstFormatI>;
};

#define BEG_INST_I(mnemonic)                                            \
    struct mnemonic: InstFormatI<EncodingTable::mnemonic::opcode,       \
                                 EncodingTable::mnemonic::funct3> {     \
        mnemonic(const u32 encoding, const u32 pc, const u32 reg[32]):  \
            InstFormatI(encoding, pc, reg) {}
#define END_INST_I(mnemonic)                                            \
    };


template <u32 opcode_, u32 funct3_>
struct InstFormatS: Instruction {
	static constexpr u32 opcode = opcode_;
	static constexpr u32 funct3 = funct3_;

    u32 rs1, rs2, imm; // imm length: 12
    u32 result;
    InstFormatS(const u32 encoding, const u32 pc, const u32 reg[32]):
        Instruction(encoding, pc),
        rs1(getbits<19, 15>(encoding)),
        rs2(getbits<24, 20>(encoding)),
        imm((getbits<31, 25>(encoding) << 5)
           + getbits<11, 7>(encoding)) {
        rs1 = reg[rs1];
        rs2 = reg[rs2];
    }

    static auto Decode(const u32 encoding, const u32 pc, const u32 reg[32])
        -> std::unique_ptr<InstFormatS>;
};

#define BEG_INST_S(mnemonic)                                            \
    struct mnemonic: InstFormatS<EncodingTable::mnemonic::opcode,       \
                                 EncodingTable::mnemonic::funct3> {     \
        mnemonic(const u32 encoding, const u32 pc, const u32 reg[32]):  \
            InstFormatS(encoding, pc, reg) {}
#define END_INST_S(mnemonic)                                            \
    };


template <u32 opcode_, u32 funct3_>
struct InstFormatB: Instruction {
	static constexpr u32 opcode = opcode_;
	static constexpr u32 funct3 = funct3_;

    u32 rs1, rs2, imm; // imm length: 13
    bool result;
    InstFormatB(const u32 encoding, const u32 pc, const u32 reg[32]):
        Instruction(encoding, pc),
        rs1(getbits<19, 15>(encoding)),
        rs2(getbits<24, 20>(encoding)),
        imm((getbits<31>(encoding) << 12)
          + (getbits<7>(encoding) << 11)
          + (getbits<30, 25>(encoding) << 5)
          + (getbits<11, 8>(encoding) << 1)
        ) {
        rs1 = reg[rs1];
        rs2 = reg[rs2];
    }

    static auto Decode(const u32 encoding, const u32 pc, const u32 reg[32])
        -> std::unique_ptr<InstFormatB>;
};

#define BEG_INST_B(mnemonic)                                            \
    struct mnemonic: InstFormatB<EncodingTable::mnemonic::opcode,       \
                                 EncodingTable::mnemonic::funct3> {     \
        mnemonic(const u32 encoding, const u32 pc, const u32 reg[32]):  \
            InstFormatB(encoding, pc, reg) {}
#define END_INST_B(mnemonic)                                            \
    };


template <u32 opcode_>
struct InstFormatU: Instruction {
	static constexpr u32 opcode = opcode_;

    u32 rd, imm; // imm length: 20
    u32 result;
    InstFormatU(const u32 encoding, const u32 pc, const u32 reg[32]):
        Instruction(encoding, pc),
        rd(getbits<11, 7>(encoding)),
        imm(getbits<31, 12>(encoding) << 12) {}

    static auto Decode(const u32 encoding, const u32 pc, const u32 reg[32])
        -> std::unique_ptr<InstFormatU>;
};

#define BEG_INST_U(mnemonic)                                            \
    struct mnemonic: InstFormatU<EncodingTable::mnemonic::opcode> {     \
        mnemonic(const u32 encoding, const u32 pc, const u32 reg[32]):  \
            InstFormatU(encoding, pc, reg) {}
#define END_INST_U(mnemonic)                                            \
    };


template <u32 opcode_>
struct InstFormatJ: Instruction {
	static constexpr u32 opcode = opcode_;

    u32 rd, imm; // imm length: 20
    u32 result;
    InstFormatJ(const u32 encoding, const u32 pc, const u32 reg[32]):
        Instruction(encoding, pc),
        rd(getbits<11, 7>(encoding)),
        imm((getbits<31>(encoding) << 20)
          + (getbits<19, 12>(encoding) << 12)
          + (getbits<20>(encoding) << 11)
          + (getbits<30, 21>(encoding) << 1)
        ) {}

    static auto Decode(const u32 encoding, const u32 pc, const u32 reg[32])
        -> std::unique_ptr<InstFormatJ>;
};

#define BEG_INST_J(mnemonic)                                            \
    struct mnemonic: InstFormatJ<EncodingTable::mnemonic::opcode> {     \
        mnemonic(const u32 encoding, const u32 pc, const u32 reg[32]):  \
            InstFormatJ(encoding, pc, reg) {}
#define END_INST_J(mnemonic)                                            \
    };


/* ---------- Instructions ---------- */

// 2.4 Integer Computational Instructions

// 2.4.1 Integer Register-Immediate Instructions

BEG_INST_I(ADDI)
    auto Execute() -> void { result = cast<i32>(rs1) + cast<i32>(SignedExt32<12>(imm)); }
END_INST_I(ADDI)

BEG_INST_I(SLTI)
    auto Execute() -> void { result = (cast<i32>(rs1) < cast<i32>(SignedExt32<12>(imm))) ? 1 : 0; }
END_INST_I(SLTI)

BEG_INST_I(SLTIU)
    auto Execute() -> void { result = (rs1 < SignedExt32<12>(imm)) ? 1 : 0; }
END_INST_I(SLTIU)

BEG_INST_I(ANDI)
    auto Execute() -> void { result = rs1 & SignedExt32<12>(imm); }
END_INST_I(ANDI)

BEG_INST_I(ORI)
    auto Execute() -> void { result = rs1 | SignedExt32<12>(imm); }
END_INST_I(ORI)

BEG_INST_I(XORI)
    auto Execute() -> void { result = rs1 ^ SignedExt32<12>(imm); }
END_INST_I(XORI)

BEG_INST_I(SLLI)
    auto Execute() -> void { result = rs1 << imm; }
END_INST_I(SLLI)

BEG_INST_I(SRLI)
    auto Execute() -> void { result = rs1 >> imm; }
END_INST_I(SRLI)

BEG_INST_I(SRAI)
    auto Execute() -> void { result = SignedExt32(rs1 >> imm, 32 - imm); }
END_INST_I(SRAI)

BEG_INST_U(LUI)
    auto Execute() -> void { result = imm; }
END_INST_U(LUI)

BEG_INST_U(AUIPC)
    auto Execute() -> void { result = pc + imm; }
END_INST_U(AUIPC)

// 2.4.2 Integer Register-Register Operations

BEG_INST_R(ADD)
    auto Execute() -> void { result = rs1 + rs2; }
END_INST_R(ADD)

BEG_INST_R(SLT)
    auto Execute() -> void { result = (cast<i32>(rs1) < cast<i32>(rs2)) ? 1 : 0; }
END_INST_R(SLT)

BEG_INST_R(SLTU)
    auto Execute() -> void { result = (rs1 < rs2) ? 1 : 0; }
END_INST_R(SLTU)

BEG_INST_R(AND)
    auto Execute() -> void { result = rs1 & rs2; }
END_INST_R(AND)

BEG_INST_R(OR)
    auto Execute() -> void { result = rs1 | rs2; }
END_INST_R(OR)

BEG_INST_R(XOR)
    auto Execute() -> void { result = rs1 ^ rs2; }
END_INST_R(XOR)

BEG_INST_R(SLL)
    auto Execute() -> void { result = rs1 << (rs2 & 31); }
END_INST_R(SLL)

BEG_INST_R(SRL)
    auto Execute() -> void { result = rs1 >> (rs2 & 31); }
END_INST_R(SRL)

BEG_INST_R(SUB)
    auto Execute() -> void { result = rs1 - rs2; }
END_INST_R(SUB)

BEG_INST_R(SRA)
    auto Execute() -> void { result = SignedExt32(rs1 >> (rs2 & 31), 32 - (rs2 & 31)); }
END_INST_R(SRA)

// 2.5 Control Transfer Instructions

// 2.5.1 Unconditional Jumps

BEG_INST_J(JAL)
    auto Execute() -> void { result = pc + 4; }
    // auto WriteBack() -> void { pc = ; }
END_INST_J(JAL)

BEG_INST_I(JALR)
    auto Execute() -> void { result = pc + 4; }
END_INST_I(JALR)


// 2.5.2 Conditional Branches

BEG_INST_B(BEQ)
    auto Execute() -> void { result = (rs1 == rs2); }
END_INST_B(BEQ)

BEG_INST_B(BNE)
    auto Execute() -> void { result = (rs1 != rs2); }
END_INST_B(BNE)

BEG_INST_B(BLT)
    auto Execute() -> void { result = (cast<i32>(rs1) < cast<i32>(rs2)); }
END_INST_B(BLT)

BEG_INST_B(BLTU)
    auto Execute() -> void { result = (rs1 < rs2); }
END_INST_B(BLTU)

BEG_INST_B(BGE)
    auto Execute() -> void { result = (cast<i32>(rs1) > cast<i32>(rs2)); }
END_INST_B(BGE)

BEG_INST_B(BGEU)
    auto Execute() -> void { result = (rs1 > rs2); }
END_INST_B(BGEU)

// 2.6 Load and Store Instructions

BEG_INST_I(LB)
    auto Execute() -> void { result = rs1 + cast<i32>(SignedExt32<12>(imm)); }
    // auto MemAccess() -> void { result = result; }
END_INST_I(LB)

BEG_INST_I(LH)
    auto Execute() -> void { result = rs1 + cast<i32>(SignedExt32<12>(imm)); }
    // auto MemAccess() -> void { result = result; }
END_INST_I(LH)

BEG_INST_I(LW)
    auto Execute() -> void { result = rs1 + cast<i32>(SignedExt32<12>(imm)); }
    // auto MemAccess() -> void { result = result; }
END_INST_I(LW)

BEG_INST_I(LBU)
    auto Execute() -> void { result = rs1 + cast<i32>(SignedExt32<12>(imm)); }
    // auto MemAccess() -> void { result = result; }
END_INST_I(LBU)

BEG_INST_I(LHU)
    auto Execute() -> void { result = rs1 + cast<i32>(SignedExt32<12>(imm)); }
    // auto MemAccess() -> void { result = result; }
END_INST_I(LHU)

BEG_INST_S(SB)
    auto Execute() -> void { result = rs1 + cast<i32>(SignedExt32<12>(imm)); }
    // auto MemAccess() -> void { result = result; }
END_INST_S(SB)

BEG_INST_S(SH)
    auto Execute() -> void { result = rs1 + cast<i32>(SignedExt32<12>(imm)); }
    // auto MemAccess() -> void { result = result; }
END_INST_S(SH)

BEG_INST_S(SW)
    auto Execute() -> void { result = rs1 + cast<i32>(SignedExt32<12>(imm)); }
    // auto MemAccess() -> void { result = result; }
END_INST_S(SW)

/* --------- undef --------- */

#undef BEG_INST_R
#undef END_INST_R

#undef BEG_INST_I
#undef END_INST_I

#undef BEG_INST_S
#undef END_INST_S

#undef BEG_INST_B
#undef END_INST_B

#undef BEG_INST_U
#undef END_INST_U

#undef BEG_INST_J
#undef END_INST_J
