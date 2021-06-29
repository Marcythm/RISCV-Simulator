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
        printf("# encoding: [%02x,%02x,%02x,%02x], pc: %x\n",
            getbits<7, 0>(encoding), getbits<15, 8>(encoding),
            getbits<23, 16>(encoding), getbits<31, 24>(encoding), pc);
    }
};

//===----------------------------------------------------------------------===//
// Instruction Formats
//===----------------------------------------------------------------------===//

struct InstFormatR: Instruction {
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
        // $rd, $rs1, $rs2
        AlignedPrintf<DumpOptions::ArgstrAlign>("%s, %s, %s", regname[rd], regname[rs1], regname[rs2]);
        Instruction::dump();
    }
};

struct InstFormatI: Instruction {
    u32 rs1, rd, imm12; // imm length (before ext): 12
    u32 rs1v, rdv;
    InstFormatI(const u32 encoding, const u32 pc, const u32 reg[32]):
        Instruction(encoding, pc),
        rs1(getbits<19, 15>(encoding)),
        rd(getbits<11, 7>(encoding)),
        imm12(SExt<12>(getbits<31, 20>(encoding))),
        rs1v(reg[rs1]) {}
    auto WriteBack(u32 &pc, u32 reg[32]) -> void { if (rd != 0) reg[rd] = rdv; }

    auto dump() -> void {
        // $rd, $rs1, $imm12
        AlignedPrintf<DumpOptions::ArgstrAlign>("%s, %s, 0x%x", regname[rd], regname[rs1], imm12);
        Instruction::dump();
    }
};

struct InstFormatS: Instruction {
    u32 rs1, rs2, imm12; // imm length (before ext): 12
    u32 rs1v, rs2v, addr;
    InstFormatS(const u32 encoding, const u32 pc, const u32 reg[32]):
        Instruction(encoding, pc),
        rs1(getbits<19, 15>(encoding)),
        rs2(getbits<24, 20>(encoding)),
        imm12(SExt<12>(
            (getbits<31, 25>(encoding) << 5)
           + getbits<11, 7>(encoding)
        )),
        rs1v(reg[rs1]), rs2v(reg[rs2]) {}

    auto dump() -> void {
        // $rs2, $imm12($rs1)
        AlignedPrintf<DumpOptions::ArgstrAlign>("%s, 0x%x(%s)", regname[rs2], imm12, regname[rs1]);
        Instruction::dump();
    }
};

struct InstFormatB: Instruction {
    u32 rs1, rs2, imm13; // imm length (before ext): 13
    u32 rs1v, rs2v, pcv; bool cond;
    InstFormatB(const u32 encoding, const u32 pc, const u32 reg[32]):
        Instruction(encoding, pc),
        rs1(getbits<19, 15>(encoding)),
        rs2(getbits<24, 20>(encoding)),
        imm13(SExt<13>(
            (getbits<31>(encoding) << 12)
          + (getbits<7>(encoding) << 11)
          + (getbits<30, 25>(encoding) << 5)
          + (getbits<11, 8>(encoding) << 1)
        )),
        rs1v(reg[rs1]), rs2v(reg[rs2]) {}
    auto WriteBack(u32 &pc, u32 reg[32]) -> void { if (cond) pc = pcv; }

    auto dump() -> void {
        // $rs1, $rs2, $imm13
        AlignedPrintf<DumpOptions::ArgstrAlign>("%s, %s, 0x%x", regname[rs1], regname[rs2], imm13);
        Instruction::dump();
    }
};

struct InstFormatU: Instruction {
    u32 rd, imm; // imm length (before ext): 32
    u32 rdv;
    InstFormatU(const u32 encoding, const u32 pc, const u32 reg[32]):
        Instruction(encoding, pc),
        rd(getbits<11, 7>(encoding)),
        imm(getbits<31, 12>(encoding) << 12) {}
    auto WriteBack(u32 &pc, u32 reg[32]) -> void { if (rd != 0) reg[rd] = rdv; }

    auto dump() -> void {
        // $rd, $imm20
        AlignedPrintf<DumpOptions::ArgstrAlign>("%s, 0x%x", regname[rd], imm);
        Instruction::dump();
    }
};

struct InstFormatJ: Instruction {
    u32 rd, imm21; // imm length (before ext): 21
    u32 rdv, pcv;
    InstFormatJ(const u32 encoding, const u32 pc, const u32 reg[32]):
        Instruction(encoding, pc),
        rd(getbits<11, 7>(encoding)),
        imm21(SExt<21>(
            (getbits<31>(encoding) << 20)
          + (getbits<19, 12>(encoding) << 12)
          + (getbits<20>(encoding) << 11)
          + (getbits<30, 21>(encoding) << 1)
        )) {}

    auto dump() -> void {
        // $rd, $imm21
        AlignedPrintf<DumpOptions::ArgstrAlign>("%s, 0x%x", regname[rd], imm21);
        Instruction::dump();
    }
};

//===----------------------------------------------------------------------===//
// Instruction Class Templates
//===----------------------------------------------------------------------===//

template <typename Tag, typename Fmt, bool isTerminal = true>
struct InstructionImpl: Tag, Fmt {
    using tag = Tag;
    using fmt = Fmt;
    using tag::opcode;

    InstructionImpl(const u32 encoding, const u32 pc, const u32 reg[32]):
        fmt(encoding, pc, reg) {}

    auto Execute() -> void { fmt::Execute(); }
    auto MemAccess(Memory &mem) -> void { fmt::MemAccess(mem); }
    auto WriteBack(u32 &pc, u32 reg[32]) -> void { fmt::WriteBack(pc, reg); }

    auto dump() -> void {
        if constexpr (isTerminal)
            AlignedPrintf<DumpOptions::OpcodestrAlign>("%s", tag::opcodestr);
        fmt::dump();
    }
};

#define specialize(type, func) template <> inline auto type::func

using ALU_ri        = InstructionImpl<InstTag::ALU_ri,       InstFormatI, false>;
using Shift_ri      = InstructionImpl<InstTag::Shift_ri,     InstFormatI, false>;
using ALU_rr        = InstructionImpl<InstTag::ALU_rr,       InstFormatR, false>;
using BranchCC_rri  = InstructionImpl<InstTag::BranchCC_rri, InstFormatB, false>;
using Load_ri       = InstructionImpl<InstTag::Load_ri,      InstFormatI, false>;
using Store_rri     = InstructionImpl<InstTag::Store_rri,    InstFormatS, false>;

specialize(Load_ri,   Execute) () -> void { rdv = rs1v + imm12; }
specialize(Store_rri, Execute) () -> void { addr = rs1v + imm12; }

specialize(Load_ri, dump) () -> void {
    // $rd, $imm12($rs1)
    AlignedPrintf<DumpOptions::ArgstrAlign>("%s, 0x%x(%s)", regname[rd], imm12, regname[rs1]);
    Instruction::dump();
}

//===----------------------------------------------------------------------===//
// Instructions
//===----------------------------------------------------------------------===//

using ADDI  = InstructionImpl<InstTag::ADDI,  ALU_ri>;
using SLTI  = InstructionImpl<InstTag::SLTI,  ALU_ri>;
using SLTIU = InstructionImpl<InstTag::SLTIU, ALU_ri>;
using ANDI  = InstructionImpl<InstTag::ANDI,  ALU_ri>;
using ORI   = InstructionImpl<InstTag::ORI,   ALU_ri>;
using XORI  = InstructionImpl<InstTag::XORI,  ALU_ri>;
using SLLI  = InstructionImpl<InstTag::SLLI,  Shift_ri>;
using SRLI  = InstructionImpl<InstTag::SRLI,  Shift_ri>;
using SRAI  = InstructionImpl<InstTag::SRAI,  Shift_ri>;
using LUI   = InstructionImpl<InstTag::LUI,   InstFormatU>;
using AUIPC = InstructionImpl<InstTag::AUIPC, InstFormatU>;
using ADD   = InstructionImpl<InstTag::ADD,   ALU_rr>;
using SLT   = InstructionImpl<InstTag::SLT,   ALU_rr>;
using SLTU  = InstructionImpl<InstTag::SLTU,  ALU_rr>;
using AND   = InstructionImpl<InstTag::AND,   ALU_rr>;
using OR    = InstructionImpl<InstTag::OR,    ALU_rr>;
using XOR   = InstructionImpl<InstTag::XOR,   ALU_rr>;
using SLL   = InstructionImpl<InstTag::SLL,   ALU_rr>;
using SRL   = InstructionImpl<InstTag::SRL,   ALU_rr>;
using SUB   = InstructionImpl<InstTag::SUB,   ALU_rr>;
using SRA   = InstructionImpl<InstTag::SRA,   ALU_rr>;
using JAL   = InstructionImpl<InstTag::JAL,   InstFormatJ>;
using JALR  = InstructionImpl<InstTag::JALR,  InstFormatI>;
using BEQ   = InstructionImpl<InstTag::BEQ,   BranchCC_rri>;
using BNE   = InstructionImpl<InstTag::BNE,   BranchCC_rri>;
using BLT   = InstructionImpl<InstTag::BLT,   BranchCC_rri>;
using BLTU  = InstructionImpl<InstTag::BLTU,  BranchCC_rri>;
using BGE   = InstructionImpl<InstTag::BGE,   BranchCC_rri>;
using BGEU  = InstructionImpl<InstTag::BGEU,  BranchCC_rri>;
using LB    = InstructionImpl<InstTag::LB,    Load_ri>;
using LH    = InstructionImpl<InstTag::LH,    Load_ri>;
using LW    = InstructionImpl<InstTag::LW,    Load_ri>;
using LBU   = InstructionImpl<InstTag::LBU,   Load_ri>;
using LHU   = InstructionImpl<InstTag::LHU,   Load_ri>;
using SB    = InstructionImpl<InstTag::SB,    Store_rri>;
using SH    = InstructionImpl<InstTag::SH,    Store_rri>;
using SW    = InstructionImpl<InstTag::SW,    Store_rri>;

// JALR is specialized separately because it needs an extra pcv field
template <> struct InstructionImpl<InstTag::JALR, InstFormatI, true>
    : InstTag::JALR, InstFormatI {
    using tag = InstTag::JALR;
    using fmt = InstFormatI;
    using tag::opcode;

    u32 pcv;
    InstructionImpl(const u32 encoding, const u32 pc, const u32 reg[32]):
        fmt(encoding, pc, reg) {}

    auto Execute() -> void { rdv = pc + 4; pcv = rs1v + imm12; }
    auto WriteBack(u32 &pc, u32 reg[32]) -> void {
        if (rd != 0) reg[rd] = rdv;
        pc = pcv;
    }

    auto dump() -> void {
        AlignedPrintf<DumpOptions::OpcodestrAlign>("%s", tag::opcodestr);
        // $rd, $imm12($rs1)
        AlignedPrintf<DumpOptions::ArgstrAlign>("%s, 0x%x(%s)", regname[rd], imm12, regname[rs1]);
        Instruction::dump();
    }
};

template <> inline SRAI::InstructionImpl(const u32 encoding, const u32 pc, const u32 reg[32]):
    fmt(encoding & ~(1 << 30), pc, reg) {}

specialize(ADDI,  Execute) () -> void { rdv = rs1v + imm12; }
specialize(SLTI,  Execute) () -> void { rdv = (cast<i32>(rs1v) < cast<i32>(imm12)) ? 1 : 0; }
specialize(SLTIU, Execute) () -> void { rdv = (rs1v < imm12) ? 1 : 0; }
specialize(ANDI,  Execute) () -> void { rdv = rs1v & imm12; }
specialize(ORI,   Execute) () -> void { rdv = rs1v | imm12; }
specialize(XORI,  Execute) () -> void { rdv = rs1v ^ imm12; }
specialize(SLLI,  Execute) () -> void { rdv = rs1v << imm12; }
specialize(SRLI,  Execute) () -> void { rdv = rs1v >> imm12; }
specialize(SRAI,  Execute) () -> void { rdv = SExt(rs1v >> imm12, 32 - imm12); }
specialize(LUI,   Execute) () -> void { rdv = imm; }
specialize(AUIPC, Execute) () -> void { rdv = pc + imm; }
specialize(ADD,   Execute) () -> void { rdv = rs1v + rs2v; }
specialize(SLT,   Execute) () -> void { rdv = (cast<i32>(rs1v) < cast<i32>(rs2v)) ? 1 : 0; }
specialize(SLTU,  Execute) () -> void { rdv = (rs1v < rs2v) ? 1 : 0; }
specialize(AND,   Execute) () -> void { rdv = rs1v & rs2v; }
specialize(OR,    Execute) () -> void { rdv = rs1v | rs2v; }
specialize(XOR,   Execute) () -> void { rdv = rs1v ^ rs2v; }
specialize(SLL,   Execute) () -> void { rdv = rs1v << (rs2v & 31); }
specialize(SRL,   Execute) () -> void { rdv = rs1v >> (rs2v & 31); }
specialize(SUB,   Execute) () -> void { rdv = rs1v - rs2v; }
specialize(SRA,   Execute) () -> void { rdv = SExt(rs1v >> (rs2v & 31), 32 - (rs2v & 31)); }
specialize(JAL,   Execute) () -> void { rdv = pc + 4; pcv = pc + imm21; }
// specialize(JALR,  Execute) () -> void { rdv = pc + 4; /*pcv = rs1v + imm12;*/ } // TODO: add pcv field for JALR
specialize(BEQ,   Execute) () -> void { pcv = pc + imm13; cond = (rs1v == rs2v); }
specialize(BNE,   Execute) () -> void { pcv = pc + imm13; cond = (rs1v != rs2v); }
specialize(BLT,   Execute) () -> void { pcv = pc + imm13; cond = (cast<i32>(rs1v) < cast<i32>(rs2v)); }
specialize(BLTU,  Execute) () -> void { pcv = pc + imm13; cond = (rs1v < rs2v); }
specialize(BGE,   Execute) () -> void { pcv = pc + imm13; cond = (cast<i32>(rs1v) > cast<i32>(rs2v)); }
specialize(BGEU,  Execute) () -> void { pcv = pc + imm13; cond = (rs1v > rs2v); }

specialize(LB,    MemAccess) (Memory &mem) -> void { rdv = SExt<8>(mem.load<u8>(rdv)); }
specialize(LH,    MemAccess) (Memory &mem) -> void { rdv = SExt<16>(mem.load<u16>(rdv)); }
specialize(LW,    MemAccess) (Memory &mem) -> void { rdv = mem.load<u32>(rdv); }
specialize(LBU,   MemAccess) (Memory &mem) -> void { rdv = mem.load<u8>(rdv); }
specialize(LHU,   MemAccess) (Memory &mem) -> void { rdv = mem.load<u16>(rdv); }
specialize(SB,    MemAccess) (Memory &mem) -> void { mem.store<u8>(addr, rs2v); }
specialize(SH,    MemAccess) (Memory &mem) -> void { mem.store<u16>(addr, rs2v); }
specialize(SW,    MemAccess) (Memory &mem) -> void { mem.store<u32>(addr, rs2v); }

specialize(JAL,   WriteBack) (u32 &pc, u32 reg[32]) -> void { if (rd != 0) reg[rd] = rdv; pc = pcv; }
// specialize(JALR,  WriteBack) (u32 &pc, u32 reg[32]) -> void { if (rd != 0) reg[rd] = rdv; pc = /*pcv*/ rs1v + imm12; }

/* --------- undef --------- */
