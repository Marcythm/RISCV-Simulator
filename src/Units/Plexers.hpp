#pragma once

#include "../config.hpp"
#include "../Wire.hpp"
#include "Unit.hpp"

/// input signal: inputs, sel
/// output signal: output
template <u32 Width, u32 K>
struct Mux: CombinationalLogicUnit {
  InputWire<Width> input[1 << K];
  InputWire<K> sel;
  OutputWire<Width> output;

  Mux() = default;
  ~Mux() = default;

  template <typename... Ts>
  auto setInput(const Ts&... ins) -> void {
    static_assert(sizeof...(ins) <= (1u << K));
    u32 index = 0;
    ((input[index++] = ins), ...);
  }

  auto setSel(const InputWire<K> &Sel) -> void {
    sel = Sel;
  }

  auto exec() -> void {
    output.set(input[sel.get().template to<u32>()].get());
  }
};

/// input signal: input, sel
/// output signal: outputs
template <u32 Width, u32 K>
struct DMux: CombinationalLogicUnit {
  InputWire<Width> input;
  InputWire<K> sel;
  OutputWire<Width> output[1 << K];

  DMux() = default;
  ~DMux() = default;

  auto setInput(const InputWire<Width> &in)-> void {
    input = in;
  }

  auto setSel(const InputWire<K> &Sel) -> void {
    sel = Sel;
  }

  auto exec() -> void {
    for (u32 i = 0; i < (1 << K); ++i)
      output[i].set(0);
    output[sel.get().template to<u32>()].set(input.get());
  }
};
