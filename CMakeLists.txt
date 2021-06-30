cmake_minimum_required(VERSION 3.10)
project(RISCV_simulator)

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_FLAGS "-O2")
add_executable(code src/main.cpp src/Instruction.cpp src/Executor.cpp)
