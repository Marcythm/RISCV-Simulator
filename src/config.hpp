#pragma once

#include <cstdio>
#include <cstring>
#include <cassert>

#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

#include <tuple>
#include <bitset>
#include <memory>
#include <type_traits>

using i8 = char;
using i16 = short;
using i32 = int;
using i64 = long long;

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

template <u32 XLEN>
using bits = std::bitset<XLEN>;

constexpr bool NOASSERT = false;

template <u32 Width = 32u> struct InputWire;
template <u32 Width = 32u> struct OutputWire;
