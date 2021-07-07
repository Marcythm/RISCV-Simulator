#pragma once

#include "config.hpp"
#include "Memory.hpp"
#include "RegisterFile.hpp"
#include "Signals.hpp"
#include "Instruction.hpp"

struct Executor {
  InstPtr IF, ID, EX, MEM, WB;
  Register pc;
  RegisterFile RF;
  StallSignal stallSignal;
  KillSignal killSignal;
  Memory mem;

  Executor(): mem{} {}
  Executor(std::istream &input): mem(input) {}

  auto initMem(std::istream &input) { mem.readfrom(input); }

  auto InstFetch() -> void;
  auto InstDecode() -> void;
  auto InstExecute() -> void;
  auto InstMemAccess() -> void;
  auto InstWriteBack() -> void;

  auto exec(std::istream &input) -> u32;
};
