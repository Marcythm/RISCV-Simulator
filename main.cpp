#include "config.hpp"
#include "Instruction.hpp"
#include "Executor.hpp"

auto main() -> i32 {
    Executor executor;
    executor.exec(std::cin);
    return 0;
}
