#pragma once

#include <cstdio>
#include <cstring>
#include <cassert>
#include <ctime>

#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

#include <tuple>
#include <bitset>
#include <memory>
#include <type_traits>

using i8 = char;
using i16 = short;
using i32 = int;
using i64 = long long;

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using f32 = float;
using f64 = double;

constexpr bool NOASSERT                      = false;
constexpr u32 MEMORY_SIZE                    = 0x20000;
constexpr bool UseTwoLevelAdaptivePredictor  = true;

constexpr char const * regname_[2][32] = {
  {
    "x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7",
    "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15",
    "x16", "x17", "x18", "x19", "x20", "x21", "x22", "x23",
    "x24", "x25", "x26", "x27", "x28", "x29", "x30", "x31"
  },
  {
    "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
  }
};

namespace DumpOptions {
  constexpr bool TrackMemOp             = false;    // track memory operations
  constexpr bool DumpInst               = true;    // dump instructions
  constexpr bool DumpRegState           = false;    // dump register states **every instruction**
  constexpr bool DumpRetValue           = false;    // dump return value
  constexpr bool DumpTargetAddr         = true;    // dump target address instead of offset in Branch/Jump instructions
  constexpr bool DumpTotalClockCycle    = false;    // dump total clock cycles
  constexpr bool DumpPredictionAccuracy = false;    // dump prediction accuracy
  constexpr bool DumpTotalTime          = false;    // dump total time used
  constexpr bool UseABIname             = true;     // dump registers with their ABI name
  constexpr u32 ClkLimit                = 0;        // exit after executing ClkLimit clock cycles

  constexpr u32 RegNameAlign      = 6;        // dump Regname with this align
  constexpr u32 OpcodestrAlign    = 8;        // dump opcodestr with this align
  constexpr u32 ArgstrAlign       = 24;       // dump argstr with this align
}

template <typename... Ts>
inline auto LOG(const Ts&... args) -> i32 {
  return fprintf(stderr, std::forward<const Ts &>(args)...);
}

inline auto LOG(const char *str) -> i32 {
  return fprintf(stderr, "%s", str);
}

constexpr const char *const * regname = regname_[DumpOptions::UseABIname];

struct Instruction;
using InstPtr = std::shared_ptr<Instruction>;

struct Executor;
