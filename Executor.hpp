#pragma once

#include "config.hpp"
#include "Memory.hpp"
#include "Instruction.hpp"

struct Executor {
    std::unique_ptr<Instruction> IF, ID, EX, MEM, WB;
    u32 pc;

    auto InstFetch(u32 &pc_, Memory *mem) -> void;
    auto InstDecode() -> void;
    auto Execute() -> void;
    auto MemAccess() -> void;
    auto WriteBack() -> void;
};
