#pragma once

#include "config.hpp"

template <u32 H, u32 L = H>
inline auto getbits(const u32 bits) -> u32 {
    // return bits{H - L}
    return bits & (((1u << (H - L + 1)) - 1u) << L);
}

template <u32 H, u32 L = H>
inline auto setbits(u32 &bits, const u32 value) -> void {
    // set bits{H-L} to val
    bits = (bits & ~(((1u << (H - L + 1)) - 1u) << L))
        |  ((value & ((1u << (H - L + 1)) - 1u)) << L);
}

template <u32 H, u32 L, u32 value>
inline auto match(const u32 bits) -> bool {
    return getbits<H, L>(bits) == value;
}

template <u32 length>
inline auto SignedExt32(const u32 bits) -> u32 {
    if (bits & (1 << (length - 1)))
        return bits | ~((1 << length) - 1);
    return bits;
}

inline auto SignedExt32(const u32 bits, const u32 length) -> u32 {
    if (bits & (1 << (length - 1)))
        return bits | ~((1 << length) - 1);
    return bits;
}

template <typename T, typename U>
inline auto cast(const U bits) -> T {
    return static_cast<T>(bits);
}
