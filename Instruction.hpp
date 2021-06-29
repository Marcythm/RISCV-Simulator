#pragma once

#include "config.hpp"
#include "InstTag.hpp"
#include "Memory.hpp"
#include "Utility.hpp"

struct Instruction {
    u32 encoding;
    u32 pc;

    Instruction(const u32 encoding, const u32 pc):
        encoding(encoding), pc(pc) {}

    static auto Decode(const u32 encoding, const u32 pc, const u32 reg[32])
        -> std::unique_ptr<Instruction>;

    virtual auto EX() -> void {}
    virtual auto MEM(Memory *mem) -> void {}
    virtual auto WB(u32 &pc, u32 reg[32]) -> void {}
};

/* ---------- InstFormats ---------- */

struct InstFormat {};

struct InstFormatR: Instruction, InstFormat {
    u32 rs1, rs2, rd;
    u32 res;
    InstFormatR(const u32 encoding, const u32 pc, const u32 reg[32]):
        Instruction(encoding, pc),
        rs1(getbits<19, 15>(encoding)),
        rs2(getbits<24, 20>(encoding)),
        rd(getbits<11, 7>(encoding)) {}
    auto WB(u32 &pc, u32 reg[32]) -> void { reg[rd] = res; }
};

struct InstFormatI: Instruction, InstFormat {
    u32 rs1, rd, imm; // imm length: 12
    u32 res;
    InstFormatI(const u32 encoding, const u32 pc, const u32 reg[32]):
        Instruction(encoding, pc),
        rs1(getbits<19, 15>(encoding)),
        rd(getbits<11, 7>(encoding)),
        imm(getbits<31, 20>(encoding)) {
        rs1 = reg[rs1];
    }
    auto WB(u32 &pc, u32 reg[32]) -> void { reg[rd] = res; }
};

struct InstFormatS: Instruction, InstFormat {
    u32 rs1, rs2, imm; // imm length: 12
    u32 res;
    InstFormatS(const u32 encoding, const u32 pc, const u32 reg[32]):
        Instruction(encoding, pc),
        rs1(getbits<19, 15>(encoding)),
        rs2(getbits<24, 20>(encoding)),
        imm((getbits<31, 25>(encoding) << 5)
           + getbits<11, 7>(encoding)) {
        rs1 = reg[rs1];
        rs2 = reg[rs2];
    }
};

struct InstFormatB: Instruction, InstFormat {
    u32 rs1, rs2, imm; // imm length: 13
    u32 res;
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
    auto WB(u32 &pc, u32 reg[32]) -> void { pc = res; }
};

struct InstFormatU: Instruction, InstFormat {
    u32 rd, imm; // imm length: 32
    u32 res;
    InstFormatU(const u32 encoding, const u32 pc, const u32 reg[32]):
        Instruction(encoding, pc),
        rd(getbits<11, 7>(encoding)),
        imm(getbits<31, 12>(encoding) << 12) {}
};

struct InstFormatJ: Instruction, InstFormat {
    u32 rd, imm; // imm length: 21
    u32 res;
    InstFormatJ(const u32 encoding, const u32 pc, const u32 reg[32]):
        Instruction(encoding, pc),
        rd(getbits<11, 7>(encoding)),
        imm((getbits<31>(encoding) << 20)
          + (getbits<19, 12>(encoding) << 12)
          + (getbits<20>(encoding) << 11)
          + (getbits<30, 21>(encoding) << 1)
        ) {}
};

/* ---------- Instructions ---------- */

template <typename Tag, typename Fmt>
struct InstructionImpl: Tag, Fmt {
    static_assert(std::is_base_of_v<InstFormat, Fmt>, "Fmt must be one type of InstFormats");
    InstructionImpl(const u32 encoding, const u32 pc, const u32 reg[32]):
        Fmt(encoding, pc, reg) {}

    auto EX() -> void { Fmt::EX(); }
    auto MEM(Memory *mem) -> void { Fmt::MEM(mem); }
    auto WB(u32 &pc, u32 reg[32]) -> void { Fmt::WB(pc, reg); }
};

// 2.4 Integer Computational Instructions

// 2.4.1 Integer Register-Immediate Instructions

using ADDI = InstructionImpl<InstTag::ADDI, InstFormatI>;
template <> auto ADDI::EX() -> void { res = cast<i32>(rs1) + cast<i32>(SExt32<12>(imm)); }

using SLTI = InstructionImpl<InstTag::SLTI, InstFormatI>;
template <> auto SLTI::EX() -> void { res = (cast<i32>(rs1) < cast<i32>(SExt32<12>(imm))) ? 1 : 0; }

using SLTIU = InstructionImpl<InstTag::SLTIU, InstFormatI>;
template <> auto SLTIU::EX() -> void { res = (rs1 < SExt32<12>(imm)) ? 1 : 0; }

using ANDI = InstructionImpl<InstTag::ANDI, InstFormatI>;
template <> auto ANDI::EX() -> void { res = rs1 & SExt32<12>(imm); }

using ORI = InstructionImpl<InstTag::ORI, InstFormatI>;
template <> auto ORI::EX() -> void { res = rs1 | SExt32<12>(imm); }

using XORI = InstructionImpl<InstTag::XORI, InstFormatI>;
template <> auto XORI::EX() -> void { res = rs1 ^ SExt32<12>(imm); }

using SLLI = InstructionImpl<InstTag::SLLI, InstFormatI>;
template <> auto SLLI::EX() -> void { res = rs1 << imm; }

using SRLI = InstructionImpl<InstTag::SRLI, InstFormatI>;
template <> auto SRLI::EX() -> void { res = rs1 >> imm; }

using SRAI = InstructionImpl<InstTag::SRAI, InstFormatI>;
template <> SRAI::InstructionImpl(const u32 encoding, const u32 pc, const u32 reg[32]):
    InstFormatI(encoding, pc, reg) { imm &= ~(1 << 10); }
template <> auto SRAI::EX() -> void { res = SExt32(rs1 >> imm, 32 - imm); }

using LUI = InstructionImpl<InstTag::LUI, InstFormatU>;
template <> auto LUI::EX() -> void { res = imm; }

using AUIPC = InstructionImpl<InstTag::AUIPC, InstFormatU>;
template <> auto AUIPC::EX() -> void { res = pc + imm; }

// 2.4.2 Integer Register-Register Operations

using ADD = InstructionImpl<InstTag::ADD, InstFormatR>;
template <> auto ADD::EX() -> void { res = rs1 + rs2; }

using SLT = InstructionImpl<InstTag::SLT, InstFormatR>;
template <> auto SLT::EX() -> void { res = (cast<i32>(rs1) < cast<i32>(rs2)) ? 1 : 0; }

using SLTU = InstructionImpl<InstTag::SLTU, InstFormatR>;
template <> auto SLTU::EX() -> void { res = (rs1 < rs2) ? 1 : 0; }

using AND = InstructionImpl<InstTag::AND, InstFormatR>;
template <> auto AND::EX() -> void { res = rs1 & rs2; }

using OR = InstructionImpl<InstTag::OR, InstFormatR>;
template <> auto OR::EX() -> void { res = rs1 | rs2; }

using XOR = InstructionImpl<InstTag::XOR, InstFormatR>;
template <> auto XOR::EX() -> void { res = rs1 ^ rs2; }

using SLL = InstructionImpl<InstTag::SLL, InstFormatR>;
template <> auto SLL::EX() -> void { res = rs1 << (rs2 & 31); }

using SRL = InstructionImpl<InstTag::SRL, InstFormatR>;
template <> auto SRL::EX() -> void { res = rs1 >> (rs2 & 31); }

using SUB = InstructionImpl<InstTag::SUB, InstFormatR>;
template <> auto SUB::EX() -> void { res = rs1 - rs2; }

using SRA = InstructionImpl<InstTag::SRA, InstFormatR>;
template <> auto SRA::EX() -> void { res = SExt32(rs1 >> (rs2 & 31), 32 - (rs2 & 31)); }

// 2.5 Control Transfer Instructions

// 2.5.1 Unconditional Jumps

using JAL = InstructionImpl<InstTag::JAL, InstFormatJ>;
template <> auto JAL::EX() -> void { res = pc + 4; }
template <> auto JAL::WB(u32 &pc, u32 reg[32]) -> void { pc += SExt32<21>(imm); }

using JALR = InstructionImpl<InstTag::JALR, InstFormatI>;
template <> auto JALR::EX() -> void { res = pc + 4; }
template <> auto JALR::WB(u32 &pc, u32 reg[32]) -> void { pc += SExt32<12>(imm); }


// 2.5.2 Conditional Branches

using BEQ = InstructionImpl<InstTag::BEQ, InstFormatB>;
template <> auto BEQ::EX() -> void { res = pc + (rs1 == rs2 ? SExt32<13>(imm) : 0); }

using BNE = InstructionImpl<InstTag::BNE, InstFormatB>;
template <> auto BNE::EX() -> void { res = pc + (rs1 != rs2 ? SExt32<13>(imm) : 0); }

using BLT = InstructionImpl<InstTag::BLT, InstFormatB>;
template <> auto BLT::EX() -> void { res = pc + (cast<i32>(rs1) < cast<i32>(rs2)) ? SExt32<13>(imm) : 0; }

using BLTU = InstructionImpl<InstTag::BLTU, InstFormatB>;
template <> auto BLTU::EX() -> void { res = pc + (rs1 < rs2) ? SExt32<13>(imm) : 0; }

using BGE = InstructionImpl<InstTag::BGE, InstFormatB>;
template <> auto BGE::EX() -> void { res = pc + (cast<i32>(rs1) > cast<i32>(rs2)) ? SExt32<13>(imm) : 0; }

using BGEU = InstructionImpl<InstTag::BGEU, InstFormatB>;
template <> auto BGEU::EX() -> void { res = pc + (rs1 > rs2) ? SExt32<13>(imm) : 0; }

// 2.6 Load and Store Instructions

using LB = InstructionImpl<InstTag::LB, InstFormatI>;
template <> auto LB::EX() -> void { res = rs1 + cast<i32>(SExt32<12>(imm)); }
template <> auto LB::MEM(Memory *mem) -> void { res = SExt32<8>(mem->load<u8>(res)); }

using LH = InstructionImpl<InstTag::LH, InstFormatI>;
template <> auto LH::EX() -> void { res = rs1 + cast<i32>(SExt32<12>(imm)); }
template <> auto LH::MEM(Memory *mem) -> void { res = SExt32<16>(mem->load<u16>(res)); }

using LW = InstructionImpl<InstTag::LW, InstFormatI>;
template <> auto LW::EX() -> void { res = rs1 + cast<i32>(SExt32<12>(imm)); }
template <> auto LW::MEM(Memory *mem) -> void { res = mem->load<u32>(res); }

using LBU = InstructionImpl<InstTag::LBU, InstFormatI>;
template <> auto LBU::EX() -> void { res = rs1 + cast<i32>(SExt32<12>(imm)); }
template <> auto LBU::MEM(Memory *mem) -> void { res = mem->load<u8>(res); }

using LHU = InstructionImpl<InstTag::LHU, InstFormatI>;
template <> auto LHU::EX() -> void { res = rs1 + cast<i32>(SExt32<12>(imm)); }
template <> auto LHU::MEM(Memory *mem) -> void { res = mem->load<u16>(res); }

using SB = InstructionImpl<InstTag::SB, InstFormatS>;
template <> auto SB::EX() -> void { res = rs1 + cast<i32>(SExt32<12>(imm)); }
template <> auto SB::MEM(Memory *mem) -> void { mem->save<u8>(res, rs2); }

using SH = InstructionImpl<InstTag::SH, InstFormatS>;
template <> auto SH::EX() -> void { res = rs1 + cast<i32>(SExt32<12>(imm)); }
template <> auto SH::MEM(Memory *mem) -> void { mem->save<u16>(res, rs2); }

using SW = InstructionImpl<InstTag::SW, InstFormatS>;
template <> auto SW::EX() -> void { res = rs1 + cast<i32>(SExt32<12>(imm)); }
template <> auto SW::MEM(Memory *mem) -> void { mem->save<u32>(res, rs2); }

/* --------- undef --------- */

// #undef BEG_INST
// #undef END_INST
