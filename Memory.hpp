#pragma once

#include "config.hpp"

struct Memory {
    static constexpr u32 MEMORY_SIZE = 0x20000;
    u8 mem[MEMORY_SIZE + 10];

    Memory(): mem{0} {}
    Memory(std::istream &input) { readfrom(input); }

    auto readfrom(std::istream &input) -> void {
        std::memset(mem, 0, sizeof mem);
        std::string buf;
        u8 *pos = mem; u32 value;
        while (std::getline(input, buf)) {
            if (buf[0] == '@') {
                pos = mem + std::stoi(buf.substr(1), nullptr, 16);
            } else {
                const char *cstr = buf.c_str();
                while (std::sscanf(cstr, "%X", &value) != EOF) {
                    *pos++ = static_cast<u8>(value);
                    cstr += 3;
                }
            }
        }
    }

    template <typename T>
    auto load(const u32 address) const -> T {
        return *((T*)(mem + address));
    }

    template <typename T>
    auto save(const u32 address, const T& value) -> void {
        *((T*)(mem + address)) = value;
    }
};
