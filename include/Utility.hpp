#pragma once

#include "config.hpp"

template <typename T, typename U>
constexpr inline auto cast(const U bits) -> T {
  return static_cast<T>(bits);
}

template <u32 H, u32 L = H>
constexpr inline auto getbits(const u32 bits) -> u32 {
  // return bits{H - L}
  return (bits >> L) & ((1u << (H - L + 1)) - 1u);
}

template <u32 H, u32 L, u32 value>
constexpr inline auto match(const u32 bits) -> bool {
  return getbits<H, L>(bits) == value;
}

template <u32 length>
inline auto SExt(const u32 bits) -> u32 {
  if constexpr (length >= 32u)
    return bits;
  std::bitset<32> b = bits;
  for (u32 i = length; i < 32; ++i)
    b[i] = b[length - 1];
  return b.to_ulong();
}

inline auto SExt(const u32 bits, const u32 length) -> u32 {
  if (length >= 32u)
    return bits;
  std::bitset<32> b = bits;
  for (u32 i = length; i < 32; ++i)
    b[i] = b[i - 1];
  return b.to_ulong();
}

inline auto AShiftR(const u32 bits, const u32 offset) -> u32 {
  std::bitset<32> b(bits >> offset);
  for (u32 i = 32 - offset; i < 32; ++i)
    b[i] = b[i - 1];
  return b.to_ulong();
}

/// signed comparison: less than
constexpr inline auto slt(const u32 a, const u32 b) -> bool {
  return cast<i32>(a) < cast<i32>(b);
}

/// signed comparison: greater than or equal to
constexpr inline auto sge(const u32 a, const u32 b) -> bool {
  return cast<i32>(a) >= cast<i32>(b);
}

/// unsigned comparison: less than
constexpr inline auto ult(const u32 a, const u32 b) -> bool {
  return a < b;
}

/// unsigned comparison: greater than or equal to
constexpr inline auto uge(const u32 a, const u32 b) -> bool {
  return a >= b;
}

constexpr inline auto GetOpcode(const u32 bits) -> u32 {
  return getbits<6, 0>(bits);
}

constexpr inline auto GetFunct3(const u32 bits) -> u32 {
  return getbits<14, 12>(bits);
}

constexpr inline auto GetFunct7(const u32 bits) -> u32 {
  return getbits<31, 25>(bits);
}

inline auto putn(const char c, i32 n) -> void {
  while (n-- > 0) LOG("%c", c);
}

/// call printf with args, then fill to align characters with spaces
template <i32 align, typename... Args>
inline auto AlignedLOG(const Args&... args) -> void {
  const i32 n = LOG(std::forward<const Args &>(args)...);
  putn(' ', align - n);
}
