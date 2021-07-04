#pragma once

#include "../config.hpp"
#include "../Wire.hpp"

struct Unit {
  virtual ~Unit() = default;
};

struct CombinationalLogicUnit: Unit {
  virtual ~CombinationalLogicUnit() = default;
  virtual auto exec() -> void = 0;
};

struct SequentialLogicUnit: Unit {
  virtual ~SequentialLogicUnit() = default;
  virtual auto tick() -> void = 0;
};
