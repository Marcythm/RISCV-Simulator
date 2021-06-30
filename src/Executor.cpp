#include "Executor.hpp"

auto Executor::InstFetch() -> InstPtr {
    auto instptr = std::make_shared<Instruction>(mem.load<u32>(pc), pc, reg);
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

auto Executor::DumpRegState() -> void {
    puts("dumping register states");
    puts("=============================== start ===============================");
    for (i32 i = 0; i < 32; i += 4) {
        for (i32 j = i; j < i + 4; ++j) {
            printf("| ");
            AlignedPrintf<DumpOptions::RegNameAlign>("%s:", regname[j]);
            printf("%08x ", reg[j]);
        }
        puts("|");
    }
    puts("================================ end ================================");
}

auto Executor::exec(std::istream &input) -> u32 {
    initMem(input); pc = 0;
    while (true) {
        InstPtr inst = InstFetch();
        InstDecode(inst);
        if constexpr (DumpOptions::DumpInst)
            inst->dump();
        if (inst->encoding == 0x0ff00513) break;
        InstExecute(inst);
        InstMemAccess(inst);
        InstWriteBack(inst);
        if constexpr (DumpOptions::DumpRegState)
            DumpRegState();
    }
    printf("ret: %d\n", reg[10] & 255u);
    return reg[10] & 255u;
}
