#include "config.hpp"
#include "Instruction.hpp"
#include "Executor.hpp"

auto main() -> i32 {
  u64 time = clock();
  Executor executor;
  printf("%d\n", executor.exec(std::cin));
  if constexpr (DumpOptions::DumpTotalTime) {
    f64 totalTime = f64(clock() - time) / CLOCKS_PER_SEC;
    printf("total time: %4lfms\n", totalTime * 1000);
  }
  return 0;
}
