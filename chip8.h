#pragma once

#include <array>
#include <vector>
#include <cstdint>
#include <memory>
#include <fmt/format.h>

constexpr std::size_t DISPLAY_WIDTH = 64;
constexpr std::size_t DISPLAY_HEIGHT = 32;
constexpr std::size_t STACK_SIZE = 16;
constexpr std::size_t RAM_SIZE = 0xFFF;
constexpr std::size_t PRG_RAM_OFFSET = 0x200;
constexpr std::size_t REGISTER_COUNT = 16;
constexpr uint16_t POS_STEP = 2;
constexpr std::size_t KEYBOARD_SIZE = 16;
constexpr std::size_t DIGIT_SIZE = 5;
constexpr std::size_t OPCODES = 16;

constexpr std::array<std::array<uint8_t, DIGIT_SIZE>, 16> CHIP8_NUMBERS = {
    {// 0
     {0xF0, 0x90, 0x90, 0x90, 0xF0},
     // 1
     {0x20, 0x60, 0x20, 0x20, 0x70},
     // 2
     {0xF0, 0x10, 0xF0, 0x80, 0xF0},
     // 3
     {0xF0, 0x10, 0xF0, 0x10, 0xF0},
     // 4
     {0x90, 0x90, 0xF0, 0x10, 0x10},
     // 5
     {0xF0, 0x80, 0xF0, 0x10, 0xF0},
     // 6
     {0xF0, 0x80, 0xF0, 0x90, 0xF0},
     // 7
     {0xF0, 0x10, 0x20, 0x40, 0x40},
     // 8
     {0xF0, 0x90, 0xF0, 0x90, 0xF0},
     // 9
     {0xF0, 0x90, 0xF0, 0x10, 0xF0},
     // A
     {0xF0, 0x90, 0xF0, 0x90, 0x90},
     // B
     {0xE0, 0x90, 0xE0, 0x90, 0xE0},
     // C
     {0xF0, 0x80, 0x80, 0x80, 0xF0},
     // D
     {0xE0, 0x90, 0x90, 0x90, 0xE0},
     // E
     {0xF0, 0x80, 0xF0, 0x80, 0xF0},
     // F
     {0xF0, 0x80, 0xF0, 0x80, 0x80}}};

class PositionStack
{
    std::array<uint16_t, STACK_SIZE> stack;
    std::size_t pos;

public:
    void set(uint16_t pos);
    void push(uint16_t pos);
    uint16_t get();
    uint16_t pop();
    void step();
    void back();
};

typedef std::array<std::array<bool, DISPLAY_WIDTH>, DISPLAY_HEIGHT> Chip8Display;

constexpr uint64_t FRAME_MILLIS = 1000 / 60;

class Chip8
{
    uint64_t oldMillis;
    uint64_t remMillis;

public:
    PositionStack position;
    std::array<uint8_t, REGISTER_COUNT> registers;
    Chip8Display vram;
    std::array<uint8_t, RAM_SIZE> ram;
    std::array<bool, KEYBOARD_SIZE> keyboard;
    // delta timer
    uint8_t dt;
    // sound timer
    uint8_t st;
    uint16_t i;
    bool running;

    Chip8();
    void cycle();
    void load(std::vector<uint8_t> &);
    void timer(uint64_t millis);
};