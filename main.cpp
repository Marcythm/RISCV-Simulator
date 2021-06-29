#include "config.hpp"
#include "Instruction.hpp"
#include "Executor.hpp"

auto main() -> i32 {
    Executor exec;
    exec.exec(std::cin);
    return 0;
}
