#pragma once

#include "config.hpp"

template <u32 H, u32 L = H>
inline auto getbits(const u32 bits) -> u32 {
    // return bits{H - L}
    return (bits >> L) & ((1u << (H - L + 1)) - 1u);
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
inline auto SExt32(const u32 bits) -> u32 {
    if (bits & (1 << (length - 1)))
        return bits | ~((1 << length) - 1);
    return bits;
}

inline auto SExt32(const u32 bits, const u32 length) -> u32 {
    if (bits & (1 << (length - 1)))
        return bits | ~((1 << length) - 1);
    return bits;
}

template <typename T, typename U>
inline auto cast(const U bits) -> T {
    return static_cast<T>(bits);
}

inline auto GetOpcode(const u32 bits) -> u32 {
    return getbits<6, 0>(bits);
}

inline auto GetFunct3(const u32 bits) -> u32 {
    return getbits<14, 12>(bits);
}

inline auto GetFunct7(const u32 bits) -> u32 {
    return getbits<31, 25>(bits);
}
