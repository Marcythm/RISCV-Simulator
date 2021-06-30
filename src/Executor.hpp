#pragma once

#include "config.hpp"
#include "Memory.hpp"
#include "Instruction.hpp"

struct Executor {
    InstPtr IF, IF_ID, ID_EX, EX_MEM, MEM_WB;
    u32 pc, reg[32];
    Memory mem;

    Executor(): pc(0), reg{0}, mem{} {}
    Executor(std::istream &input): pc(0), reg{0}, mem(input) {}

    auto initMem(std::istream &input) { mem.readfrom(input); }

    auto DumpRegState() -> void;

    auto InstFetch() -> void;
    auto InstDecode() -> void;
    auto InstExecute() -> void;
    auto InstMemAccess() -> void;
    auto InstWriteBack() -> void;

    auto exec(std::istream &input) -> u32;
};
