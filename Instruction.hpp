#pragma once

#include "config.hpp"
#include "InstTag.hpp"
#include "Memory.hpp"
#include "Utility.hpp"

struct Instruction;
using InstPtr = std::shared_ptr<Instruction>;

struct Instruction {
    u32 encoding;
    u32 pc;

    Instruction(const u32 encoding, const u32 pc):
        encoding(encoding), pc(pc) {}

    static auto Decode(const u32 encoding, const u32 pc, const u32 reg[32])
        -> InstPtr;

    virtual auto Execute() -> void {}
    virtual auto MemAccess(Memory &mem) -> void {}
    virtual auto WriteBack(u32 &pc, u32 reg[32]) -> void {}

    virtual auto dump() -> void {
        printf("\t\t");
        printf("# encoding: [%02x,%02x,%02x,%02x], pc: %x\n",
            getbits<7, 0>(encoding), getbits<15, 8>(encoding),
            getbits<23, 16>(encoding), getbits<31, 24>(encoding), pc);
    }
};

/* ---------- InstFormats ---------- */

struct InstFormat {};

struct InstFormatR: Instruction, InstFormat {
    u32 rs1, rs2, rd;
    u32 rs1v, rs2v, rdv;
    InstFormatR(const u32 encoding, const u32 pc, const u32 reg[32]):
        Instruction(encoding, pc),
        rs1(getbits<19, 15>(encoding)),
        rs2(getbits<24, 20>(encoding)),
        rd(getbits<11, 7>(encoding)),
        rs1v(reg[rs1]), rs2v(reg[rs2]) {}
    auto WriteBack(u32 &pc, u32 reg[32]) -> void { if (rd != 0) reg[rd] = rdv; }

    auto dump() -> void {
        printf("\t");
        printf("%s, %s, %s", regname[rd], regname[rs1], regname[rs2]); // $rd, $rs1, $rs2
        Instruction::dump();
    }
};

struct InstFormatI: Instruction, InstFormat {
    u32 rs1, rd, imm12; // imm length: 12
    u32 rs1v, rdv;
    InstFormatI(const u32 encoding, const u32 pc, const u32 reg[32]):
        Instruction(encoding, pc),
        rs1(getbits<19, 15>(encoding)),
        rd(getbits<11, 7>(encoding)),
        imm12(getbits<31, 20>(encoding)),
        rs1v(reg[rs1]) {}
    auto WriteBack(u32 &pc, u32 reg[32]) -> void { if (rd != 0) reg[rd] = rdv; }

    auto dump() -> void {
        printf("\t");
        printf("%s, %s, %d", regname[rd], regname[rs1], imm12); // $rd, $rs1, $imm12
        Instruction::dump();
    }
};

struct InstFormatS: Instruction, InstFormat {
    u32 rs1, rs2, imm12; // imm length: 12
    u32 rs1v, rs2v, addr;
    InstFormatS(const u32 encoding, const u32 pc, const u32 reg[32]):
        Instruction(encoding, pc),
        rs1(getbits<19, 15>(encoding)),
        rs2(getbits<24, 20>(encoding)),
        imm12((getbits<31, 25>(encoding) << 5)
           + getbits<11, 7>(encoding)),
        rs1v(reg[rs1]), rs2v(reg[rs2]) {}

    auto dump() -> void {
        printf("\t");
        printf("%s, %d(%s)", regname[rs2], imm12, regname[rs1]); // $rs2, $imm12($rs1)
        Instruction::dump();
    }
};

struct InstFormatB: Instruction, InstFormat {
    u32 rs1, rs2, imm13; // imm length: 13
    u32 rs1v, rs2v, pcv;
    InstFormatB(const u32 encoding, const u32 pc, const u32 reg[32]):
        Instruction(encoding, pc),
        rs1(getbits<19, 15>(encoding)),
        rs2(getbits<24, 20>(encoding)),
        imm13((getbits<31>(encoding) << 12)
          + (getbits<7>(encoding) << 11)
          + (getbits<30, 25>(encoding) << 5)
          + (getbits<11, 8>(encoding) << 1)),
        rs1v(reg[rs1]), rs2v(reg[rs2]) {}
    auto WriteBack(u32 &pc, u32 reg[32]) -> void { pc = pcv; }

    auto dump() -> void {
        printf("\t");
        printf("%s, %s, %d", regname[rs1], regname[rs2], imm13); // $rs1, $rs2, $imm13
        Instruction::dump();
    }
};

struct InstFormatU: Instruction, InstFormat {
    u32 rd, imm; // imm length: 32
    u32 rdv;
    InstFormatU(const u32 encoding, const u32 pc, const u32 reg[32]):
        Instruction(encoding, pc),
        rd(getbits<11, 7>(encoding)),
        imm(getbits<31, 12>(encoding) << 12) {}
    auto WriteBack(u32 &pc, u32 reg[32]) -> void { if (rd != 0) reg[rd] = rdv; }

    auto dump() -> void {
        printf("\t");
        printf("%s, %d", regname[rd], imm); // $rd, $imm20
        Instruction::dump();
    }
};

struct InstFormatJ: Instruction, InstFormat {
    u32 rd, imm21; // imm length: 21
    u32 rdv, pcv;
    InstFormatJ(const u32 encoding, const u32 pc, const u32 reg[32]):
        Instruction(encoding, pc),
        rd(getbits<11, 7>(encoding)),
        imm21((getbits<31>(encoding) << 20)
          + (getbits<19, 12>(encoding) << 12)
          + (getbits<20>(encoding) << 11)
          + (getbits<30, 21>(encoding) << 1)
        ) {}

    auto dump() -> void {
        printf("\t");
        printf("%s, %d", regname[rd], imm21); // $rd, $imm21
        Instruction::dump();
    }
};

/* ---------- Instructions ---------- */

template <typename Tag, typename Fmt>
struct InstructionImpl: Tag, Fmt {
    static_assert(std::is_base_of_v<InstFormat, Fmt>, "Fmt must be one type of InstFormats");
    InstructionImpl(const u32 encoding, const u32 pc, const u32 reg[32]):
        Fmt(encoding, pc, reg) {}

    auto Execute() -> void { Fmt::Execute(); }
    auto MemAccess(Memory &mem) -> void { Fmt::MemAccess(mem); }
    auto WriteBack(u32 &pc, u32 reg[32]) -> void { Fmt::WriteBack(pc, reg); }

    auto dump() -> void { printf("%s", Tag::opcodestr); Fmt::dump(); }
};

using ADDI  = InstructionImpl<InstTag::ADDI,  InstFormatI>;
using SLTI  = InstructionImpl<InstTag::SLTI,  InstFormatI>;
using SLTIU = InstructionImpl<InstTag::SLTIU, InstFormatI>;
using ANDI  = InstructionImpl<InstTag::ANDI,  InstFormatI>;
using ORI   = InstructionImpl<InstTag::ORI,   InstFormatI>;
using XORI  = InstructionImpl<InstTag::XORI,  InstFormatI>;
using SLLI  = InstructionImpl<InstTag::SLLI,  InstFormatI>;
using SRLI  = InstructionImpl<InstTag::SRLI,  InstFormatI>;
using SRAI  = InstructionImpl<InstTag::SRAI,  InstFormatI>;
using LUI   = InstructionImpl<InstTag::LUI,   InstFormatU>;
using AUIPC = InstructionImpl<InstTag::AUIPC, InstFormatU>;
using ADD   = InstructionImpl<InstTag::ADD,   InstFormatR>;
using SLT   = InstructionImpl<InstTag::SLT,   InstFormatR>;
using SLTU  = InstructionImpl<InstTag::SLTU,  InstFormatR>;
using AND   = InstructionImpl<InstTag::AND,   InstFormatR>;
using OR    = InstructionImpl<InstTag::OR,    InstFormatR>;
using XOR   = InstructionImpl<InstTag::XOR,   InstFormatR>;
using SLL   = InstructionImpl<InstTag::SLL,   InstFormatR>;
using SRL   = InstructionImpl<InstTag::SRL,   InstFormatR>;
using SUB   = InstructionImpl<InstTag::SUB,   InstFormatR>;
using SRA   = InstructionImpl<InstTag::SRA,   InstFormatR>;
using JAL   = InstructionImpl<InstTag::JAL,   InstFormatJ>;
using JALR  = InstructionImpl<InstTag::JALR,  InstFormatI>;
using BEQ   = InstructionImpl<InstTag::BEQ,   InstFormatB>;
using BNE   = InstructionImpl<InstTag::BNE,   InstFormatB>;
using BLT   = InstructionImpl<InstTag::BLT,   InstFormatB>;
using BLTU  = InstructionImpl<InstTag::BLTU,  InstFormatB>;
using BGE   = InstructionImpl<InstTag::BGE,   InstFormatB>;
using BGEU  = InstructionImpl<InstTag::BGEU,  InstFormatB>;
using LB    = InstructionImpl<InstTag::LB,    InstFormatI>;
using LH    = InstructionImpl<InstTag::LH,    InstFormatI>;
using LW    = InstructionImpl<InstTag::LW,    InstFormatI>;
using LBU   = InstructionImpl<InstTag::LBU,   InstFormatI>;
using LHU   = InstructionImpl<InstTag::LHU,   InstFormatI>;
using SB    = InstructionImpl<InstTag::SB,    InstFormatS>;
using SH    = InstructionImpl<InstTag::SH,    InstFormatS>;
using SW    = InstructionImpl<InstTag::SW,    InstFormatS>;

template <> inline SRAI::InstructionImpl(const u32 encoding, const u32 pc, const u32 reg[32]):
    InstFormatI(encoding & ~(1 << 30), pc, reg) {}

template <> inline auto ADDI  ::Execute() -> void { rdv = cast<i32>(rs1v) + cast<i32>(SExt<12>(imm12)); }
template <> inline auto SLTI  ::Execute() -> void { rdv = (cast<i32>(rs1v) < cast<i32>(SExt<12>(imm12))) ? 1 : 0; }
template <> inline auto SLTIU ::Execute() -> void { rdv = (rs1v < SExt<12>(imm12)) ? 1 : 0; }
template <> inline auto ANDI  ::Execute() -> void { rdv = rs1v & SExt<12>(imm12); }
template <> inline auto ORI   ::Execute() -> void { rdv = rs1v | SExt<12>(imm12); }
template <> inline auto XORI  ::Execute() -> void { rdv = rs1v ^ SExt<12>(imm12); }
template <> inline auto SLLI  ::Execute() -> void { rdv = rs1v << imm12; }
template <> inline auto SRLI  ::Execute() -> void { rdv = rs1v >> imm12; }
template <> inline auto SRAI  ::Execute() -> void { rdv = SExt(rs1v >> imm12, 32 - imm12); }
template <> inline auto LUI   ::Execute() -> void { rdv = imm; }
template <> inline auto AUIPC ::Execute() -> void { rdv = pc + imm; }
template <> inline auto ADD   ::Execute() -> void { rdv = rs1v + rs2v; }
template <> inline auto SLT   ::Execute() -> void { rdv = (cast<i32>(rs1v) < cast<i32>(rs2v)) ? 1 : 0; }
template <> inline auto SLTU  ::Execute() -> void { rdv = (rs1v < rs2v) ? 1 : 0; }
template <> inline auto AND   ::Execute() -> void { rdv = rs1v & rs2v; }
template <> inline auto OR    ::Execute() -> void { rdv = rs1v | rs2v; }
template <> inline auto XOR   ::Execute() -> void { rdv = rs1v ^ rs2v; }
template <> inline auto SLL   ::Execute() -> void { rdv = rs1v << (rs2v & 31); }
template <> inline auto SRL   ::Execute() -> void { rdv = rs1v >> (rs2v & 31); }
template <> inline auto SUB   ::Execute() -> void { rdv = rs1v - rs2v; }
template <> inline auto SRA   ::Execute() -> void { rdv = SExt(rs1v >> (rs2v & 31), 32 - (rs2v & 31)); }
template <> inline auto JAL   ::Execute() -> void { rdv = pc + 4; pcv = pc + SExt<21>(imm21); }
template <> inline auto JALR  ::Execute() -> void { rdv = pc + 4; /*pcv = rs1v + SExt<12>(imm12);*/ } // TODO: add pcv field for JALR
template <> inline auto BEQ   ::Execute() -> void { pcv = pc + (rs1v == rs2v ? SExt<13>(imm13) : 0); }
template <> inline auto BNE   ::Execute() -> void { pcv = pc + (rs1v != rs2v ? SExt<13>(imm13) : 0); }
template <> inline auto BLT   ::Execute() -> void { pcv = pc + ((cast<i32>(rs1v) < cast<i32>(rs2v)) ? SExt<13>(imm13) : 0); }
template <> inline auto BLTU  ::Execute() -> void { pcv = pc + ((rs1v < rs2v) ? SExt<13>(imm13) : 0); }
template <> inline auto BGE   ::Execute() -> void { pcv = pc + ((cast<i32>(rs1v) > cast<i32>(rs2v)) ? SExt<13>(imm13) : 0); }
template <> inline auto BGEU  ::Execute() -> void { pcv = pc + ((rs1v > rs2v) ? SExt<13>(imm13) : 0); }
template <> inline auto LB    ::Execute() -> void { rdv = rs1v + cast<i32>(SExt<12>(imm12)); }
template <> inline auto LH    ::Execute() -> void { rdv = rs1v + cast<i32>(SExt<12>(imm12)); }
template <> inline auto LW    ::Execute() -> void { rdv = rs1v + cast<i32>(SExt<12>(imm12)); }
template <> inline auto LBU   ::Execute() -> void { rdv = rs1v + cast<i32>(SExt<12>(imm12)); }
template <> inline auto LHU   ::Execute() -> void { rdv = rs1v + cast<i32>(SExt<12>(imm12)); }
template <> inline auto SB    ::Execute() -> void { addr = rs1v + cast<i32>(SExt<12>(imm12)); }
template <> inline auto SH    ::Execute() -> void { addr = rs1v + cast<i32>(SExt<12>(imm12)); }
template <> inline auto SW    ::Execute() -> void { addr = rs1v + cast<i32>(SExt<12>(imm12)); }

template <> inline auto LB    ::MemAccess(Memory &mem) -> void { rdv = SExt<8>(mem.load<u8>(rdv)); }
template <> inline auto LH    ::MemAccess(Memory &mem) -> void { rdv = SExt<16>(mem.load<u16>(rdv)); }
template <> inline auto LW    ::MemAccess(Memory &mem) -> void { rdv = mem.load<u32>(rdv); }
template <> inline auto LBU   ::MemAccess(Memory &mem) -> void { rdv = mem.load<u8>(rdv); }
template <> inline auto LHU   ::MemAccess(Memory &mem) -> void { rdv = mem.load<u16>(rdv); }
template <> inline auto SB    ::MemAccess(Memory &mem) -> void { mem.save<u8>(addr, rs2v); }
template <> inline auto SH    ::MemAccess(Memory &mem) -> void { mem.save<u16>(addr, rs2v); }
template <> inline auto SW    ::MemAccess(Memory &mem) -> void { mem.save<u32>(addr, rs2v); }

template <> inline auto JAL   ::WriteBack(u32 &pc, u32 reg[32]) -> void { if (rd != 0) reg[rd] = rdv; pc = pcv; }
template <> inline auto JALR  ::WriteBack(u32 &pc, u32 reg[32]) -> void { if (rd != 0) reg[rd] = rdv; pc = /*pcv*/ rs1v + SExt<12>(imm12); }


/* --------- undef --------- */
