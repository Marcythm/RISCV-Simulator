cmake_minimum_required(VERSION 3.10)
project(RISCV_simulator)

# SET (CMAKE_CXX_COMPILER  "/usr/local/bin/g++-11")
set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_FLAGS "-Og -g")
set(CMAKE_CXX_FLAGS "-O2")

include_directories(include)

add_executable(code lib/main.cpp lib/Instruction.cpp lib/Executor.cpp)
