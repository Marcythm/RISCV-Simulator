#pragma once

#include "config.hpp"
#include "EncodingTable.hpp"
#include "Memory.hpp"
#include "Utility.hpp"

struct Instruction {
    u32 encoding;
    u32 pc;

    Instruction(const u32 encoding_, const u32 pc_):
        encoding(encoding_), pc(pc_) {}

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

    u8 rs1, rs2, rd;
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

    u8 rs1, rd; u16 imm; // imm length: 12
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

    u8 rs1, rs2; u16 imm; // imm length: 12
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

    u8 rs1, rs2; u16 imm; // imm length: 13
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

    u8 rd; u32 imm; // imm length: 20
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

    u8 rd; u32 imm; // imm length: 20
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
