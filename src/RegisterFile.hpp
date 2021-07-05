#pragma once

#include "config.hpp"

struct Register {
  u32 input, output;

  Register(): input(0), output(0) {};
  Register(const u32 value): input(0), output(value) {};
  ~Register() = default;

  operator u32() const { return output; }
  auto operator= (const u32 &rhs) -> void {
    input = rhs;
  }

  auto tick() -> void { output = input; }
};

struct RegisterFile {
  Register x[32];

  RegisterFile() = default;
  ~RegisterFile() = default;

  auto operator[] (const u32 &sel) -> Register& {
    return x[sel];
  }
  auto operator[] (const u32 &sel) const -> const Register& {
    return x[sel];
  }

  auto tick() -> void {
    [this]<u32... Index>(std::integer_sequence<u32, Index...>) {
      ((x[Index].tick()), ...);
    }(std::make_integer_sequence<u32, 31>{});
  }
};
