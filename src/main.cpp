#include "config.hpp"
#include "Instruction.hpp"
#include "Executor.hpp"

auto main() -> i32 {
    Executor executor;
    printf("%d\n", executor.exec(std::cin));
    return 0;
}
