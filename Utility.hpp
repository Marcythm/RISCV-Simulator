#pragma once

#include "config.hpp"

template <u32 H, u32 L = H>
constexpr inline auto getbits(const u32 bits) -> u32 {
    // return bits{H - L}
    return (bits >> L) & ((1u << (H - L + 1)) - 1u);
}

template <u32 H, u32 L = H>
constexpr inline auto setbits(u32 &bits, const u32 value) -> void {
    // set bits{H-L} to val
    bits = (bits & ~(((1u << (H - L + 1)) - 1u) << L))
        |  ((value & ((1u << (H - L + 1)) - 1u)) << L);
}

template <u32 H, u32 L, u32 value>
constexpr inline auto match(const u32 bits) -> bool {
    return getbits<H, L>(bits) == value;
}

template <u32 slength, u32 tlength = 32>
constexpr inline auto SExt(const u32 bits) -> u32 {
    if constexpr (tlength >= 32) {
        if (bits & (1 << (slength - 1)))
            return bits | ~((1 << slength) - 1);
        return bits;
    } else {
        if (bits & (1 << (slength - 1)))
            return bits | (~((1 << slength) - 1) & ((1 << tlength) - 1));
        return bits;
    }
}

constexpr inline auto SExt(const u32 bits, const u32 slength, const u32 tlength = 32)
     -> u32 {
    if (bits & (1 << (slength - 1)))
        return bits | (~((1 << slength) - 1) & ((1 << tlength) - 1));
    return bits;
}

template <typename T, typename U>
constexpr inline auto cast(const U bits) -> T {
    return static_cast<T>(bits);
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

inline auto putn(char c, i32 n) -> void {
    while (n-- > 0) putchar(c);
}

template <i32 align, typename... Args>
inline auto AlignedPrintf(const Args&... args) -> void {
    /// call printf with args, then fill to align characters with spaces
    const i32 n = printf(std::forward<const Args &>(args)...);
    putn(' ', align - n);
}
