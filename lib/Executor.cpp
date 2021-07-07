#include "Executor.hpp"

auto Executor::InstFetch() -> void {
  IF = std::make_shared<Instruction>(mem.load<u32>(pc), pc, RF);
  pc = pc + 4;
}

auto Executor::InstDecode() -> void {
  if (ID == nullptr)
    return;

  ID = Instruction::Decode(ID->encoding, Register(ID->pc), RF);

  if (JAL::is(ID->encoding)) {
    pc = ID->pc + ID->imm;
    killSignal.set<KillSignal::ID>();
    return;
  }

  if (BranchCC_rri::is(ID->encoding)) {
    auto inst = std::dynamic_pointer_cast<BranchCC_rri>(ID);

    if (inst == nullptr and !NOASSERT)
      assert(false);

    inst->pred = predictor.predict(ID->pc);
    if (inst->pred) {
      pc = ID->pc + ID->imm;
      killSignal.set<KillSignal::ID>();
    }
  }
}

auto Executor::InstExecute() -> void {
  if (EX == nullptr)
    return;

  EX->Execute();

  if (JALR::is(EX->encoding)) {
    auto inst = std::dynamic_pointer_cast<JALR>(EX);
    if (inst == nullptr and !NOASSERT)
      assert(false);
    pc = std::get<0>(inst->fields);
    killSignal.set<KillSignal::EX>();
  }

  if (BranchCC_rri::is(EX->encoding)) {
    auto inst = std::dynamic_pointer_cast<BranchCC_rri>(EX);

    if (inst == nullptr and !NOASSERT)
      assert(false);

    predictor.report(inst->pc, inst->cond);
    if (inst->cond != inst->pred) {
      pc = inst->cond ? inst->pcv : (inst->pc + 4);
      killSignal.set<KillSignal::EX>();
    }
  }
}

auto Executor::InstMemAccess() -> void {
  static u32 Counter = 0; // simulate memory access with 3 clock cycles
  static InstPtr inst = nullptr;

  if (Counter == 0) {
    if (MEM == nullptr)
      return;
    if (!Load_ri::is(MEM->encoding) and !Store_rri::is(MEM->encoding))
      return;
  }

  if (Counter == 0) {
    inst = MEM;
    Counter = 3;
    stallSignal.set<StallSignal::MEM>(3);
  }

  if (--Counter == 0) {
    MEM = inst;
    inst = nullptr;
    MEM->MemAccess(mem);
  } else {
    MEM = nullptr;
  }
}

auto Executor::InstWriteBack() -> void {
  if (WB == nullptr)
    return;

  WB->WriteBack(RF);
  RF.tick();
}

auto Executor::exec(std::istream &input) -> u32 {
  initMem(input);
  pc = 0; pc.tick();
  IF = ID = EX = MEM = WB = nullptr;
  for (u64 clk = 0; ; ++clk) {
    // forwarding
    if (ID and ID->rs1) {
      if (EX and EX->rd == ID->rs1 and !Load_ri::is(EX->encoding))
        ID->rs1v = EX->rdv;
      else if (MEM and MEM->rd == ID->rs1)
        ID->rs1v = MEM->rdv;
    }
    if (ID and ID->rs2) {
      if (EX and EX->rd == ID->rs2 and !Load_ri::is(EX->encoding))
        ID->rs2v = EX->rdv;
      else if (MEM and MEM->rd == ID->rs2)
        ID->rs2v = MEM->rdv;
    }

    // tick
    pc.tick();
    WB = MEM;
    MEM = EX;
    if (!stallSignal.willStall<StallSignal::EX>()) {
      EX = ID;
    } else if (stallSignal.willInsertBubble())
      EX = nullptr;
    if (!stallSignal.willStall<StallSignal::ID>())
      ID = IF;

    if (!stallSignal.willStall<StallSignal::IF>())
      InstFetch();
    InstWriteBack();
    if (!stallSignal.willStall<StallSignal::ID>())
      InstDecode();
    if (!stallSignal.willStall<StallSignal::EX>())
      InstExecute();
    InstMemAccess();

    stallSignal.countDown();
    if (stallSignal.noStall() and EX and Load_ri::is(EX->encoding))
      if (ID and EX->rd != 0 and (EX->rd == ID->rs1 or EX->rd == ID->rs2))
        stallSignal.set<StallSignal::MEM>(1, true);

    if (killSignal.willKill<KillSignal::IF>())
      IF = nullptr;
    if (killSignal.willKill<KillSignal::ID>())
      ID = nullptr;
    killSignal.reset();

    /* ----------------- Dump Options ----------------- */

    if constexpr (!NOASSERT)
      assert(u32(RF[0]) == 0);

    if constexpr (DumpOptions::DumpInst or DumpOptions::DumpRegState)
      LOG("clock cycle %llu\n", clk);

    if constexpr (DumpOptions::DumpInst) {
      LOG("IF  "); if (IF) IF->dump(); else putn(' ', 28), LOG("bubble\n");
      LOG("ID  "); if (ID) ID->dump(); else putn(' ', 28), LOG("bubble\n");
      LOG("EX  "); if (EX) EX->dump(); else putn(' ', 28), LOG("bubble\n");
      LOG("MEM "); if (MEM) MEM->dump(); else putn(' ', 28), LOG("bubble\n");
      LOG("WB  "); if (WB) WB->dump(); else putn(' ', 28), LOG("bubble\n");
      LOG("\n");
    }

    if constexpr (DumpOptions::DumpRegState) {
      RF.dump();
      LOG("\n\n");
    }

    if constexpr (DumpOptions::ClkLimit > 0) {
      if (clk >= DumpOptions::ClkLimit)
        break;
    }

    if (MEM and MEM->encoding == 0x0ff00513u) {
      LOG("=========================== Execution Ends ===========================\n");
      if constexpr (DumpOptions::DumpTotalClockCycle)
        LOG("execution time:       %llu clock cycles\n", clk);
      if constexpr (DumpOptions::DumpPredictionAccuracy)
        LOG("prediction accuracy:  %.6lf%% (%llu hits / %llu predictions in total)\n",
          predictor.hitRate() * 100.0, predictor.hit, predictor.total);
      break;
    }
  }
  if constexpr (DumpOptions::DumpRetValue)
    LOG("return value: %d\n", RF[10] & 255u);
  return RF[10] & 255u;
}
