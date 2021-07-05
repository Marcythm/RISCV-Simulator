#include "Executor.hpp"

auto Executor::InstFetch() -> void {
  IF = std::make_shared<Instruction>(mem.load<u32>(pc), pc, RF);
  pc = pc + 4;
}

auto Executor::InstDecode() -> void {
  ID = Instruction::Decode(ID->encoding, Register(ID->pc), RF);
}

auto Executor::InstExecute() -> void {
  EX->Execute();
}

auto Executor::InstMemAccess() -> void {
  MEM->MemAccess(mem);
}

auto Executor::InstWriteBack() -> void {
  WB->WriteBack(pc, RF);
}

auto Executor::DumpRegState() -> void {
  puts("dumping register states");
  puts("=============================== start ===============================");
  for (i32 i = 0; i < 32; i += 4) {
    for (i32 j = i; j < i + 4; ++j) {
      printf("| ");
      AlignedPrintf<DumpOptions::RegNameAlign>("%s:", regname[j]);
      printf("%08x ", u32(RF[j]));
    }
    puts("|");
  }
  puts("================================ end ================================");
}

auto Executor::exec(std::istream &input) -> u32 {
  initMem(input); pc = 0;
  for (u32 clk = 0; ; ++clk) {
    InstFetch(); ID = IF;
    InstDecode(); EX = ID;
    if constexpr (DumpOptions::DumpInst)
      EX->dump();
    if constexpr (DumpOptions::DumpRegState)
      DumpRegState();
    if (EX->encoding == 0x0ff00513u) break;
    InstExecute(); MEM = EX;
    InstMemAccess(); WB = MEM;
    InstWriteBack();

    pc.tick();
    RF.tick();

    if constexpr (DumpOptions::ClkLimit > 0) {
      if (clk >= DumpOptions::ClkLimit)
        break;
    }
  }
  if constexpr (DumpOptions::DumpRetValue)
    fprintf(stderr, "return value: %d\n", RF[10] & 255u);
  return RF[10] & 255u;
}
