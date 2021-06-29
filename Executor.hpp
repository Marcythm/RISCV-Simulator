#pragma once

#include "config.hpp"
#include "Memory.hpp"
#include "Instruction.hpp"

struct Executor {
    InstPtr IF, ID, EX, MEM, WB;
    u32 pc, reg[32];
    Memory mem;

    Executor(): pc(0), reg{0}, mem{} {}
    Executor(std::istream &input): pc(0), reg{0}, mem(input) {}

    auto initMem(std::istream &input) { mem.readfrom(input); }

    auto InstFetch() -> InstPtr;
    auto InstDecode(InstPtr &) -> void;
    auto InstExecute(const InstPtr &) -> void;
    auto InstMemAccess(const InstPtr &) -> void;
    auto InstWriteBack(const InstPtr &) -> void;

    auto exec(std::istream &input) -> u32;
};
