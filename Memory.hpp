#pragma once

#include "config.hpp"

struct Memory {
    static constexpr u32 MEMORY_SIZE = 0x20000;
    u8 mem[MEMORY_SIZE + 10];

    Memory(std::istream &input): mem{0} {
        std::string buf;
        u8 *pos = mem;
        while (std::getline(input, buf)) {
            if (buf[0] == '@') {
                pos = mem + std::stoi(buf.substr(1), nullptr, 16);
            } else {
                const char *cstr = buf.c_str();
                while (std::sscanf(cstr, "%X", pos++) != EOF) cstr += 3;
            }
        }
    }

    template <typename T>
    auto read(const u32 address) const -> T {
        return *static_cast<T*>(mem + address);
    }

    template <typename T>
    auto write(const u32 address, const T& value) -> void {
        *static_cast<T*>(mem + address) = value;
    }
};
