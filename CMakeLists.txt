cmake_minimum_required(VERSION 3.10)

project (Chip8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(X11 REQUIRED)
find_package(fmt)

add_executable(chip8 main.cpp chip8.cpp)

target_link_libraries(chip8 ${X11_LIBRARIES} fmt::fmt)