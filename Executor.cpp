#include "Executor.hpp"

auto Executor::InstFetch() -> InstPtr {
    auto instptr = std::make_shared<Instruction>(mem.load<u32>(pc), pc);
    pc += 4;
    return instptr;
}

auto Executor::InstDecode(InstPtr &inst) -> void {
    inst = Instruction::Decode(inst->encoding, inst->pc, reg);
}

auto Executor::InstExecute(const InstPtr &inst) -> void {
    inst->Execute();
}

auto Executor::InstMemAccess(const InstPtr &inst) -> void {
    inst->MemAccess(mem);
}

auto Executor::InstWriteBack(const InstPtr &inst) -> void {
    inst->WriteBack(pc, reg);
}

auto Executor::dumpReg() -> void {
    for (i32 i = 1; i < 32; ++i)
        if (reg[i]) printf("%s: %08x | ", regname[i], reg[i]);
    puts("\n");
}

auto Executor::exec(std::istream &input) -> u32 {
    initMem(input); pc = 0;
    while (true) {
        InstPtr inst = InstFetch();
        InstDecode(inst);
        if constexpr (DumpOptions::enabled && DumpOptions::Inst)
            inst->dump();
        if (inst->encoding == 0x0ff00513) break;
        InstExecute(inst);
        InstMemAccess(inst);
        InstWriteBack(inst);
        if constexpr (DumpOptions::enabled && DumpOptions::Reg)
            dumpReg();
    }
    printf("ret: %d\n", reg[10] & 255u);
    return reg[10] & 255u;
}
