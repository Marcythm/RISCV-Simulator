#pragma once

#include "config.hpp"
#include "InstTag.hpp"
#include "Memory.hpp"
#include "RegisterFile.hpp"
#include "Utility.hpp"

struct Instruction {
  u32 encoding;
  u32 pc;
  u32 rs1, rs2, rd, imm;
  u32 rs1v, rs2v, rdv;

  Instruction(const u32 encoding, const Register &pc, const RegisterFile &RF):
    encoding(encoding), pc(pc),
    rs1(getbits<19, 15>(encoding)),
    rs2(getbits<24, 20>(encoding)),
    rd(getbits<11, 7>(encoding)),
    rs1v(RF[rs1]), rs2v(RF[rs2]) {}
  virtual ~Instruction() {};

  static auto Decode(const u32 encoding, const Register &pc, const RegisterFile &RF)
    -> InstPtr;

  virtual auto Execute() -> void {}
  virtual auto MemAccess(Memory &) -> void {}
  virtual auto WriteBack(RegisterFile &)-> void {}

  auto dumpPCAndEncoding() -> void {
    printf("%5x: %02x %02x %02x %02x", pc,
      getbits<7, 0>(encoding), getbits<15, 8>(encoding),
      getbits<23, 16>(encoding), getbits<31, 24>(encoding));
  }
  virtual auto dumpOpcodestr() -> void {
    AlignedPrintf<DumpOptions::OpcodestrAlign>("%s", "unknown");
  }
  virtual auto dumpArgstr() -> void {
    AlignedPrintf<DumpOptions::ArgstrAlign>("%s", "");
  }
  auto dump() -> void {
    this->dumpPCAndEncoding();
    putn(' ', 10);
    this->dumpOpcodestr();
    this->dumpArgstr();
    printf("\n");
  }
};

//===----------------------------------------------------------------------===//
// Instruction Formats
//===----------------------------------------------------------------------===//

struct InstFormatR: Instruction {
  InstFormatR(const u32 encoding, const Register &pc, const RegisterFile &RF):
    Instruction(encoding, pc, RF) {}
  ~InstFormatR() {}

  auto WriteBack(RegisterFile &RF) -> void { RF[rd] = rdv; }
  auto dumpArgstr() -> void {
    // $rd, $rs1, $rs2
    AlignedPrintf<DumpOptions::ArgstrAlign>("%s, %s, %s", regname[rd], regname[rs1], regname[rs2]);
  }
};

struct InstFormatI: Instruction {
  u32 imm12; // imm length (before ext): 12
  InstFormatI(const u32 encoding, const Register &pc, const RegisterFile &RF):
    Instruction(encoding, pc, RF),
    imm12(SExt<12>(getbits<31, 20>(encoding))) {
      rs2 = 0;
      rs2v = 0;
      imm = imm12;
    }
  ~InstFormatI() {}

  auto WriteBack(RegisterFile &RF) -> void { RF[rd] = rdv; }
  auto dumpArgstr() -> void {
    // $rd, $rs1, $imm12
    AlignedPrintf<DumpOptions::ArgstrAlign>("%s, %s, 0x%x", regname[rd], regname[rs1], imm12);
  }
};

struct InstFormatS: Instruction {
  u32 imm12; // imm length (before ext): 12
  u32 addr;
  InstFormatS(const u32 encoding, const Register &pc, const RegisterFile &RF):
    Instruction(encoding, pc, RF),
    imm12(SExt<12>(
      (getbits<31, 25>(encoding) << 5)
    + (getbits<11, 7>(encoding))
    )) {
      rd = 0;
      imm = imm12;
    }
  ~InstFormatS() {}

  auto dumpArgstr() -> void {
    // $rs2, $imm12($rs1)
    AlignedPrintf<DumpOptions::ArgstrAlign>("%s, 0x%x(%s)", regname[rs2], imm12, regname[rs1]);
  }
};

struct InstFormatB: Instruction {
  u32 imm13; // imm length (before ext): 13
  u32 pcv; bool cond;
  InstFormatB(const u32 encoding, const Register &pc, const RegisterFile &RF):
    Instruction(encoding, pc, RF),
    imm13(SExt<13>(
      (getbits<31>(encoding) << 12)
    + (getbits<7>(encoding) << 11)
    + (getbits<30, 25>(encoding) << 5)
    + (getbits<11, 8>(encoding) << 1)
    )) {
      rd = 0;
      imm = imm13;
    }
  ~InstFormatB() {}

  auto dumpArgstr() -> void {
    // $rs1, $rs2, $imm13
    if constexpr (DumpOptions::DumpTargetAddr)
      AlignedPrintf<DumpOptions::ArgstrAlign>("%s, %s, 0x%x", regname[rs1], regname[rs2], pc + imm13);
    else
      AlignedPrintf<DumpOptions::ArgstrAlign>("%s, %s, 0x%x", regname[rs1], regname[rs2], imm13);
  }
};

struct InstFormatU: Instruction {
  u32 imm; // imm length (before ext): 32
  InstFormatU(const u32 encoding, const Register &pc, const RegisterFile &RF):
    Instruction(encoding, pc, RF),
    imm(getbits<31, 12>(encoding) << 12) {
      rs1 = rs2 = 0;
      rs1v = rs2v = 0;
      Instruction::imm = imm;
    }
  ~InstFormatU() {}

  auto WriteBack(RegisterFile &RF) -> void { RF[rd] = rdv; }
  auto dumpArgstr() -> void {
    // $rd, $imm20
    AlignedPrintf<DumpOptions::ArgstrAlign>("%s, 0x%x", regname[rd], imm);
  }
};

struct InstFormatJ: Instruction {
  u32 imm21; // imm length (before ext): 21
  u32 pcv;
  InstFormatJ(const u32 encoding, const Register &pc, const RegisterFile &RF):
    Instruction(encoding, pc, RF),
    imm21(SExt<21>(
      (getbits<31>(encoding) << 20)
    + (getbits<19, 12>(encoding) << 12)
    + (getbits<20>(encoding) << 11)
    + (getbits<30, 21>(encoding) << 1)
    )) {
      rs1 = rs2 = 0;
      rs1v = rs2v = 0;
      imm = imm21;
    }
  ~InstFormatJ() {}

  auto WriteBack(RegisterFile &RF) -> void { RF[rd] = rdv; }
  auto dumpArgstr() -> void {
    // $rd, $imm21
    if constexpr (DumpOptions::DumpTargetAddr)
      AlignedPrintf<DumpOptions::ArgstrAlign>("%s, 0x%x", regname[rd], pc + imm21);
    else
      AlignedPrintf<DumpOptions::ArgstrAlign>("%s, 0x%x", regname[rd], imm21);
  }
};

//===----------------------------------------------------------------------===//
// Instruction Class Templates
//===----------------------------------------------------------------------===//

template <typename Type, typename Fmt, bool isTerminal = true, typename... AdditionalFields>
struct InstructionImpl: Type, Fmt {
  using type = Type;
  using fmt = Fmt;
  using type::opcode;

  std::tuple<AdditionalFields...> fields;
  InstructionImpl(const u32 encoding, const Register &pc, const RegisterFile &RF):
    fmt(encoding, pc, RF) {}
  ~InstructionImpl() {}

  static auto is(const u32 encoding) -> bool { return GetOpcode(encoding) == opcode; }

  auto Execute() -> void { fmt::Execute(); }
  auto MemAccess(Memory &mem) -> void { fmt::MemAccess(mem); }
  auto WriteBack(RegisterFile &RF) -> void { fmt::WriteBack(RF); }

  auto dumpOpcodestr() -> void {
    if constexpr (isTerminal)
      AlignedPrintf<DumpOptions::OpcodestrAlign>("%s", type::opcodestr);
  }
  auto dumpArgstr() -> void {
    Fmt::dumpArgstr();
  }
};

template <typename Type, typename Fmt, bool isTerminal>
struct InstructionImpl<Type, Fmt, isTerminal>: Type, Fmt {
  using type = Type;
  using fmt = Fmt;
  using type::opcode;

  InstructionImpl(const u32 encoding, const Register &pc, const RegisterFile &RF):
    fmt(encoding, pc, RF) {}
  ~InstructionImpl() {}

  static auto is(const u32 encoding) -> bool { return GetOpcode(encoding) == opcode; }

  auto Execute() -> void { fmt::Execute(); }
  auto MemAccess(Memory &mem) -> void { fmt::MemAccess(mem); }
  auto WriteBack(RegisterFile &RF) -> void { fmt::WriteBack(RF); }

  auto dumpOpcodestr() -> void {
    if constexpr (isTerminal)
      AlignedPrintf<DumpOptions::OpcodestrAlign>("%s", type::opcodestr);
  }
  auto dumpArgstr() -> void {
    Fmt::dumpArgstr();
  }
};

#define specialize(type, func) template <> inline auto type::func

using ALU_ri        = InstructionImpl<InstTag::ALU_ri,       InstFormatI, false>;
using Shift_ri      = InstructionImpl<InstTag::Shift_ri,     InstFormatI, false>;
using ALU_rr        = InstructionImpl<InstTag::ALU_rr,       InstFormatR, false>;
using BranchCC_rri  = InstructionImpl<InstTag::BranchCC_rri, InstFormatB, false>;
using Load_ri       = InstructionImpl<InstTag::Load_ri,      InstFormatI, false>;
using Store_rri     = InstructionImpl<InstTag::Store_rri,    InstFormatS, false>;

template <> inline Shift_ri::InstructionImpl(const u32 encoding, const Register &pc, const RegisterFile &RF):
  fmt(encoding, pc, RF) { imm12 &= 0b11111u; }

specialize(Load_ri,   Execute) () -> void { rdv = rs1v + imm12; }
specialize(Store_rri, Execute) () -> void { addr = rs1v + imm12; }

specialize(Load_ri, dumpArgstr) () -> void {
  // $rd, $imm12($rs1)
  AlignedPrintf<DumpOptions::ArgstrAlign>("%s, 0x%x(%s)", regname[rd], imm12, regname[rs1]);
}

//===---------------------------------------------------------------------===//
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
using SUB   = InstructionImpl<InstTag::SUB,   ALU_rr>;
using SLT   = InstructionImpl<InstTag::SLT,   ALU_rr>;
using SLTU  = InstructionImpl<InstTag::SLTU,  ALU_rr>;
using AND   = InstructionImpl<InstTag::AND,   ALU_rr>;
using OR    = InstructionImpl<InstTag::OR,    ALU_rr>;
using XOR   = InstructionImpl<InstTag::XOR,   ALU_rr>;
using SLL   = InstructionImpl<InstTag::SLL,   ALU_rr>;
using SRL   = InstructionImpl<InstTag::SRL,   ALU_rr>;
using SRA   = InstructionImpl<InstTag::SRA,   ALU_rr>;
using JAL   = InstructionImpl<InstTag::JAL,   InstFormatJ>;
using JALR  = InstructionImpl<InstTag::JALR,  InstFormatI, true, u32>;
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

using Unknown = InstructionImpl<InstTag::Unknown, Instruction>;

specialize(ADDI,  Execute) () -> void { rdv = rs1v + imm12; }
specialize(SLTI,  Execute) () -> void { rdv = slt(rs1v, imm12) ? 1 : 0; }
specialize(SLTIU, Execute) () -> void { rdv = ult(rs1v, imm12) ? 1 : 0; }
specialize(ANDI,  Execute) () -> void { rdv = rs1v & imm12; }
specialize(ORI,   Execute) () -> void { rdv = rs1v | imm12; }
specialize(XORI,  Execute) () -> void { rdv = rs1v ^ imm12; }
specialize(SLLI,  Execute) () -> void { rdv = rs1v << imm12; }
specialize(SRLI,  Execute) () -> void { rdv = rs1v >> imm12; }
specialize(SRAI,  Execute) () -> void { rdv = AShiftR(rs1v, imm12); }
specialize(LUI,   Execute) () -> void { rdv = imm; }
specialize(AUIPC, Execute) () -> void { rdv = pc + imm; }
specialize(ADD,   Execute) () -> void { rdv = rs1v + rs2v; }
specialize(SUB,   Execute) () -> void { rdv = rs1v - rs2v; }
specialize(SLT,   Execute) () -> void { rdv = slt(rs1v, rs2v) ? 1 : 0; }
specialize(SLTU,  Execute) () -> void { rdv = ult(rs1v, rs2v) ? 1 : 0; }
specialize(AND,   Execute) () -> void { rdv = rs1v & rs2v; }
specialize(OR,    Execute) () -> void { rdv = rs1v | rs2v; }
specialize(XOR,   Execute) () -> void { rdv = rs1v ^ rs2v; }
specialize(SLL,   Execute) () -> void { rdv = rs1v << (rs2v & 0b11111u); }
specialize(SRL,   Execute) () -> void { rdv = rs1v >> (rs2v & 0b11111u); }
specialize(SRA,   Execute) () -> void { rdv = AShiftR(rs1v, rs2v & 0b11111u); }
specialize(JAL,   Execute) () -> void { rdv = pc + 4; pcv = pc + imm21; }
specialize(JALR,  Execute) () -> void { rdv = pc + 4; std::get<0>(fields) = (rs1v + imm12) & ~1u; }
specialize(BEQ,   Execute) () -> void { pcv = pc + imm13; cond = (rs1v == rs2v); }
specialize(BNE,   Execute) () -> void { pcv = pc + imm13; cond = (rs1v != rs2v); }
specialize(BLT,   Execute) () -> void { pcv = pc + imm13; cond = slt(rs1v, rs2v); }
specialize(BLTU,  Execute) () -> void { pcv = pc + imm13; cond = ult(rs1v, rs2v); }
specialize(BGE,   Execute) () -> void { pcv = pc + imm13; cond = sge(rs1v, rs2v); }
specialize(BGEU,  Execute) () -> void { pcv = pc + imm13; cond = uge(rs1v, rs2v); }

specialize(LB,    MemAccess) (Memory &mem) -> void { rdv = SExt<8>(mem.load<u8>(rdv)); }
specialize(LH,    MemAccess) (Memory &mem) -> void { rdv = SExt<16>(mem.load<u16>(rdv)); }
specialize(LW,    MemAccess) (Memory &mem) -> void { rdv = mem.load<u32>(rdv); }
specialize(LBU,   MemAccess) (Memory &mem) -> void { rdv = mem.load<u8>(rdv); }
specialize(LHU,   MemAccess) (Memory &mem) -> void { rdv = mem.load<u16>(rdv); }
specialize(SB,    MemAccess) (Memory &mem) -> void { mem.store<u8>(addr, cast<u8>(rs2v)); }
specialize(SH,    MemAccess) (Memory &mem) -> void { mem.store<u16>(addr, cast<u16>(rs2v)); }
specialize(SW,    MemAccess) (Memory &mem) -> void { mem.store<u32>(addr, cast<u32>(rs2v)); }

specialize(JALR,  dumpOpcodestr) () -> void {
  AlignedPrintf<DumpOptions::OpcodestrAlign>("%s", type::opcodestr);
}

specialize(JALR,  dumpArgstr) () -> void {
  // $rd, $imm12($rs1)
  AlignedPrintf<DumpOptions::ArgstrAlign>("%s, 0x%x(%s)", regname[rd], imm12, regname[rs1]);
}


/* --------- undef --------- */
