cmake_minimum_required(VERSION 3.10)
project(RISCV_simulator)

# SET (CMAKE_CXX_COMPILER  "/usr/local/bin/g++-11")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-O2")
add_executable(code src/main.cpp src/Instruction.cpp src/Executor.cpp)
