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

constexpr std::size_t VRAM_SIZE = 32;
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
    std::array<uint64_t, VRAM_SIZE> vram;
    std::array<uint8_t, RAM_SIZE> ram;
};