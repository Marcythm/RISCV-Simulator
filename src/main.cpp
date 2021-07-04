#include "Bits.hpp"
#include "Wire.hpp"
#include "Units/Unit.hpp"
#include "Units/Plexers.hpp"
#include "Units/RegisterFile.hpp"

Bits<32> x, y;
Mux<32, 1> mux;
RegisterFile reg;

auto main() -> i32 {
  x[1u] = 1;
  reg.wdata.get();
  y[1u] = 0;
  y[3] = 1;
  printf("%u\n", (x + y).to<u32>());
}
