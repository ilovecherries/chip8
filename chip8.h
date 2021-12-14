#include <array>
#include <cstdint>

class Chip8;

class Opcode
{
public:
    virtual void opcode(Chip8 &chip8, uint16_t ins);
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

constexpr std::size_t DISPLAY_WIDTH = 64;
constexpr std::size_t DISPLAY_HEIGHT = 32;
constexpr std::size_t STACK_SIZE = 16;
constexpr std::size_t RAM_SIZE = 0xFFF;
constexpr std::size_t PRG_RAM_OFFSET = 0x200;
constexpr std::size_t REGISTER_COUNT = 16;
constexpr uint16_t POS_STEP = 2;

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

class Chip8
{
public:
    PositionStack position;
    std::array<uint8_t, REGISTER_COUNT> registers;
    std::array<std::array<bool, DISPLAY_WIDTH>, DISPLAY_HEIGHT> vram;
    std::array<uint8_t, RAM_SIZE> ram;
    uint16_t i;
};