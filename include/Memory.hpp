#pragma once

#include "config.hpp"

struct Memory {
  u8 mem[MEMORY_SIZE + 10];

  Memory(): mem{0} {}
  Memory(std::istream &input) { readfrom(input); }

  auto readfrom(std::istream &input) -> void {
    if constexpr (DumpOptions::TrackMemOp)
      puts("---------- loading memory ----------");
    std::memset(mem, 0, sizeof mem);
    std::string buf;
    u8 *pos = mem; u32 value;
    while (std::getline(input, buf)) {
      if (buf[0] == '@') {
        pos = mem + std::stoi(buf.substr(1), nullptr, 16);
      } else {
        std::stringstream ss(buf); ss << std::hex;
        while (ss >> value) {
          *pos++ = static_cast<u8>(value);
        }
      }
    }
    if constexpr (DumpOptions::TrackMemOp)
      puts("---------- memory loaded ----------");
  }

  template <typename T>
  auto load(const u32 address) const -> T {
    if constexpr (!NOASSERT)
      assert(address < MEMORY_SIZE && "load address exceeds MEMORY_SIZE");
    if constexpr (DumpOptions::TrackMemOp)
      printf("load from memory: addr = %08x, value = %08x\n", address, *((T*)(mem + address)));
    return *((T*)(mem + address));
  }

  template <typename T>
  auto store(const u32 address, const T &value) -> void {
    if constexpr (!NOASSERT)
      assert(address < MEMORY_SIZE && "store address exceeds MEMORY_SIZE");
    if constexpr (DumpOptions::TrackMemOp)
      printf("store to memory:   addr = %08x, value = %08x\n", address, value);
    *((T*)(mem + address)) = value;
  }
};
