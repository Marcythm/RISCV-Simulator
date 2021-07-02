#pragma once

#include "config.hpp"

template <u32 Width>
struct Bits {
  constexpr Bits() noexcept = default;
  constexpr Bits(const u32 value) noexcept: impl(value) {}

  auto operator== (const Bits &rhs) const noexcept -> bool {
    return impl == rhs.impl;
  }
  auto operator!= (const Bits &rhs) const noexcept -> bool {
    return impl != rhs.impl;
  }

  constexpr auto operator[] (const u32 pos) const -> bool {
    return impl[pos];
  }
  auto operator[] (u32 pos) -> typename std::bitset<Width>::reference {
    return impl[pos];
  }

  auto operator&= (const Bits &rhs) noexcept -> Bits& {
    impl &= rhs.impl;
    return *this;
  }
  auto operator|= (const Bits &rhs) noexcept -> Bits& {
    impl |= rhs.impl;
    return *this;
  }
  auto operator^= (const Bits &rhs) noexcept -> Bits& {
    impl ^= rhs.impl;
    return *this;
  }
  auto operator~ () const noexcept -> Bits {
    return ~impl;
  }

  auto operator<< (const u32 pos) const noexcept -> Bits {
    return Bits(impl << pos);
  }
  auto operator<<= (const u32 pos) noexcept -> Bits& {
    impl <<= pos;
    return *this;
  }
  auto operator>> (const u32 pos) const noexcept -> Bits {
    return Bits(impl >> pos);
  }
  auto operator>>= (const u32 pos) noexcept -> Bits& {
    impl >>= pos;
    return *this;
  }

  auto operator+= (const Bits &rhs) noexcept -> Bits& {
    return *this = *this + rhs;
  }
  auto operator-= (const Bits &rhs) noexcept -> Bits& {
    return *this = *this - rhs;
  }

  template <typename T>
  auto to() const noexcept -> T {
    if constexpr (std::is_same_v<T, std::string>)
      return impl.to_string();
    if constexpr (std::is_same_v<T, u32>)
      return impl.to_ulong();
    if constexpr (std::is_same_v<T, i32>)
      return static_cast<i32>(impl.to_ulong());
  }

  auto toString(const char zero = '0', const char one = '1')
    const noexcept -> std::string {
    return impl.to_string(zero, one);
  }

private:
  std::bitset<Width> impl;

  constexpr Bits(std::bitset<Width> bits): impl(bits) {}

  friend auto operator& (const Bits &lhs, const Bits &rhs) noexcept -> Bits {
    return lhs.impl & rhs.impl;
  }
  friend auto operator| (const Bits &lhs, const Bits &rhs) noexcept -> Bits {
    return lhs.impl | rhs.impl;
  }
  friend auto operator^ (const Bits &lhs, const Bits &rhs) noexcept -> Bits {
    return lhs.impl ^ rhs.impl;
  }

  friend auto operator+ (const Bits &lhs, const Bits &rhs) noexcept -> Bits {
    const u32 lval = lhs.to<u32>();
    const u32 rval = rhs.to<u32>();
    return Bits(lval + rval);
  }
  friend auto operator- (const Bits &lhs, const Bits &rhs) noexcept -> Bits {
    const u32 lval = lhs.to<u32>();
    const u32 rval = rhs.to<u32>();
    return Bits(lval - rval);
  }

  friend auto operator< (const Bits &lhs, const Bits &rhs) noexcept -> bool {
    return lhs.to<u32>() < rhs.to<u32>();
  }
  friend auto operator<= (const Bits &lhs, const Bits &rhs) noexcept -> bool {
    return lhs.to<u32>() <= rhs.to<u32>();
  }
  friend auto operator> (const Bits &lhs, const Bits &rhs) noexcept -> bool {
    return lhs.to<u32>() > rhs.to<u32>();
  }
  friend auto operator>= (const Bits &lhs, const Bits &rhs) noexcept -> bool {
    return lhs.to<u32>() >= rhs.to<u32>();
  }
  friend auto lt(const Bits &lhs, const Bits &rhs) noexcept -> bool {
    return lhs.to<i32>() < rhs.to<i32>();
  }
  friend auto le(const Bits &lhs, const Bits &rhs) noexcept -> bool {
    return lhs.to<i32>() <= rhs.to<i32>();
  }
  friend auto gt(const Bits &lhs, const Bits &rhs) noexcept -> bool {
    return lhs.to<i32>() > rhs.to<i32>();
  }
  friend auto ge(const Bits &lhs, const Bits &rhs) noexcept -> bool {
    return lhs.to<i32>() >= rhs.to<i32>();
  }
};
