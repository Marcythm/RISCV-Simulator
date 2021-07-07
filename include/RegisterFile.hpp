#pragma once

#include "config.hpp"
#include "Utility.hpp"

struct Register {
  Register(): input(0), output(0) {};
  explicit Register(const u32 value): input(0), output(value) {};
  ~Register() = default;

  operator u32() const { return output; }
  auto operator= (const u32 rhs) -> Register& {
    input = rhs;
    return *this;
  }

  auto tick() -> void { output = input; }

private:
  u32 input, output;
};

struct RegisterFile {
  Register x[32];

  RegisterFile() = default;
  ~RegisterFile() = default;

  auto operator[] (const u32 sel) -> Register& {
    return x[sel];
  }
  auto operator[] (const u32 sel) const -> const Register& {
    return x[sel];
  }

  auto tick() -> void {
    [this]<u32... Index>(std::integer_sequence<u32, Index...>) {
      ((x[Index + 1].tick()), ...);
    }(std::make_integer_sequence<u32, 31>{});
  }

  auto dump() -> void {
    LOG("dumping register states\n");
    LOG("=============================== start ===============================\n");
    for (i32 i = 0; i < 32; i += 4) {
      for (i32 j = i; j < i + 4; ++j) {
        LOG("| ");
        AlignedLOG<DumpOptions::RegNameAlign>("%s:", regname[j]);
        LOG("%08x ", u32(x[j]));
      }
      LOG("|\n");
    }
    LOG("================================ end ================================\n");
  }
};
