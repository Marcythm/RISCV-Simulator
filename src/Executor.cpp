#include "Executor.hpp"

auto Executor::InstFetch() -> void {
    IF = std::make_shared<Instruction>(mem.load<u32>(pc), pc, reg);
    pc += 4;
}

auto Executor::InstDecode() -> void {
    IF_ID = Instruction::Decode(IF_ID->encoding, IF_ID->pc, reg);
}

auto Executor::InstExecute() -> void {
    ID_EX->Execute();
}

auto Executor::InstMemAccess() -> void {
    EX_MEM->MemAccess(mem);
}

auto Executor::InstWriteBack() -> void {
    MEM_WB->WriteBack(pc, reg);
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
        InstFetch(); IF_ID = IF;
        InstDecode(); ID_EX = IF_ID;
        if constexpr (DumpOptions::DumpInst)
            ID_EX->dump();
        if (ID_EX->encoding == 0x0ff00513u) break;
        InstExecute(); EX_MEM = ID_EX;
        InstMemAccess(); MEM_WB = EX_MEM;
        InstWriteBack();
        if constexpr (DumpOptions::DumpRegState)
            DumpRegState();
    }
    if constexpr (DumpOptions::DumpRetValue)
        printf("return value: %d\n", reg[10] & 255u);
    return reg[10] & 255u;
}
