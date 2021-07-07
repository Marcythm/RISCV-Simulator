#pragma once

#include "config.hpp"

struct SaturatingCounter {
  u8 prediction;

  SaturatingCounter() = default;
  ~SaturatingCounter() = default;

  auto predict() const -> bool {
    return prediction >= 2;
  }

  auto report(const bool taken) -> bool {
    if (taken) {
      if (prediction < 3)
        ++prediction;
      return prediction == 3;
    } else {
      if (prediction > 0)
        --prediction;
      return prediction == 0;
    }
  }
};

template <u32 N>
struct TwoLevelAdaptivePredictor {
  SaturatingCounter counter[MEMORY_SIZE << (N - 2)];
  u8 history[MEMORY_SIZE >> 2];

  TwoLevelAdaptivePredictor() = default;
  ~TwoLevelAdaptivePredictor() = default;

  auto getCounterEntry(const u32 pc) const
    -> const SaturatingCounter & {
    return counter[(pc << (N - 2)) + history[pc >> 2]];
  }

  auto getCounterEntry(const u32 pc)
    -> SaturatingCounter & {
    return counter[(pc << (N - 2)) + history[pc >> 2]];
  }

  auto updateHistory(const u32 pc, const bool taken) -> void {
    history[pc >> 2] = ((history[pc >> 2] << 1) | (taken ? 1 : 0)) & ((1u << N) - 1u);
  }

  auto predict(const u32 pc) const -> bool {
    return getCounterEntry(pc).predict();
  }

  auto report(const u32 pc, const bool taken) -> bool {
    const bool hit = getCounterEntry(pc).report(taken);
    updateHistory(pc, taken);
    return hit;
  }
};

struct Predictor {
  TwoLevelAdaptivePredictor<2> impl;

  Predictor() = default;
  ~Predictor() = default;

  mutable u64 hit, total;
  auto predict(const u32 pc) const -> bool {
    ++total;
    if constexpr (UseTwoLevelAdaptivePredictor)
      return impl.predict(pc);
    else
      return false;
  }

  auto report(const u32 pc, const bool taken) -> void {
    if constexpr (UseTwoLevelAdaptivePredictor) {
        if (impl.report(pc, taken))
          ++hit;
    } else {
      if (taken)
        ++hit;
    }
  }

  auto hitRate() const -> f64 {
    return total == 0 ? 0 : (f64)hit / (f64)total;
  }
};
