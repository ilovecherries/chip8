#include <array>
#include <vector>
#include <cstdint>

class Chip8;

class Opcode
{
public:
    virtual void opcode(Chip8 &chip8, uint16_t ins) = 0;
};

class SYS_Opcode : public Opcode
{
public:
    void opcode(Chip8 &chip8, uint16_t ins);
};

class JMP_Opcode : public Opcode
{
public:
    void opcode(Chip8 &chip8, uint16_t ins);
};

class CALL_Opcode : public Opcode
{
public:
    void opcode(Chip8 &chip8, uint16_t ins);
};

class SE_Opcode : public Opcode
{
public:
    void opcode(Chip8 &chip8, uint16_t ins);
};

class SNE_Opcode : public Opcode
{
public:
    void opcode(Chip8 &chip8, uint16_t ins);
};

class SE_R_Opcode : public Opcode
{
public:
    void opcode(Chip8 &chip8, uint16_t ins);
};

class LD_Opcode : public Opcode
{
public:
    void opcode(Chip8 &chip8, uint16_t ins);
};

class ADD_Opcode : public Opcode
{
public:
    void opcode(Chip8 &chip8, uint16_t ins);
};

class REG_Opcode : public Opcode
{
public:
    void opcode(Chip8 &chip8, uint16_t ins);
};

class SNE_R_Opcode : public Opcode
{
public:
    void opcode(Chip8 &chip8, uint16_t ins);
};

class LD_NNN_Opcode : public Opcode
{
public:
    void opcode(Chip8 &chip8, uint16_t ins);
};

class JMP_R_Opcode : public Opcode
{
public:
    void opcode(Chip8 &chip8, uint16_t ins);
};

class RND_Opcode : public Opcode
{
public:
    void opcode(Chip8 &chip8, uint16_t ins);
};

class DRW_Opcode : public Opcode
{
public:
    void opcode(Chip8 &chip8, uint16_t ins);
};

class SKP_K_Opcode : public Opcode
{
public:
    void opcode(Chip8 &chip8, uint16_t ins);
};

class LD_T_Opcode : public Opcode
{
public:
    void opcode(Chip8 &chip8, uint16_t ins);
};

constexpr std::size_t DISPLAY_WIDTH = 64;
constexpr std::size_t DISPLAY_HEIGHT = 32;
constexpr std::size_t STACK_SIZE = 16;
constexpr std::size_t RAM_SIZE = 0xFFF;
constexpr std::size_t PRG_RAM_OFFSET = 0x200;
constexpr std::size_t REGISTER_COUNT = 16;
constexpr uint16_t POS_STEP = 2;
constexpr std::size_t KEYBOARD_SIZE = 16;
constexpr std::size_t DIGIT_SIZE = 5;

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
};

typedef std::array<std::array<bool, DISPLAY_WIDTH>, DISPLAY_HEIGHT> Chip8Display;

class Chip8
{
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

    Chip8();
    void cycle();
    void load(std::vector<uint8_t> &);
};