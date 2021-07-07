#pragma once

#include "config.hpp"

namespace {
  template <u32 Pos>
  struct StageTag {
    static constexpr u32 pos = Pos;
  };

  template <u32 a, u32 b>
  constexpr auto operator< (const StageTag<a> &, const StageTag<b> &)
    -> bool { return a < b; }
}

struct StallSignal {
  using IF  = StageTag<1>;
  using ID  = StageTag<2>;
  using EX  = StageTag<3>;
  using MEM = StageTag<4>;

  u32 stallPos;
  u32 stallTimeCount;
  bool insertBubble;

  StallSignal() = default;
  ~StallSignal() = default;

  template <typename StallStage>
  auto set(const u32 time, const bool bubble = false) -> void {
    stallPos = StallStage::pos;
    stallTimeCount = time;
    insertBubble = bubble;
  }

  auto noStall() const -> bool {
    return stallTimeCount == 0;
  }

  template <typename curStage>
  auto willStall() const -> bool {
    return curStage::pos < stallPos;
  }

  auto willInsertBubble() const -> bool {
    return insertBubble;
  }

  auto countDown() -> void {
    if (stallPos == 0)
      return;
    if (--stallTimeCount == 0)
      stallPos = 0;
  }
};

struct KillSignal {
  using IF  = StageTag<1>;
  using ID  = StageTag<2>;
  using EX  = StageTag<3>;

  KillSignal() = default;
  ~KillSignal() = default;

  u32 killPos;

  template <typename StallStage>
  auto set() -> void {
    killPos = StallStage::pos;
  }

  auto reset() -> void {
    killPos = 0;
  }

  template <typename curStage>
  auto willKill() const -> bool {
    return curStage::pos < killPos;
  }
};
