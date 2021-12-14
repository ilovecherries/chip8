cmake_minimum_required(VERSION 3.10)

project (Chip8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(chip8 main.cpp chip8.cpp)