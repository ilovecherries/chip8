#include "chip8.h"
#include <stdexcept>
#include <tuple>
#include <memory>
#include <cstdlib>
#include <fmt/format.h>

// helper functions for getting values out of opcode instructions
/**/
uint16_t nnn(uint16_t ins)
{
    return 0xFFF & ins;
}

std::tuple<uint8_t, uint8_t> xkk(uint16_t ins)
{
    uint8_t x = (ins & 0xF00) >> 8;
    uint8_t kk = ins & 0xFF;
    return std::make_tuple(x, kk);
}

std::tuple<uint8_t, uint8_t> xy0(uint16_t ins)
{
    uint8_t x = (ins & 0xF00) >> 8;
    uint8_t y = (ins & 0xF0) >> 4;
    return std::make_tuple(x, y);
}

std::tuple<uint8_t, uint8_t, uint8_t> xyz(uint16_t ins)
{
    uint8_t x = (ins & 0xF00) >> 8;
    uint8_t y = (ins & 0xF0) >> 4;
    uint8_t z = (ins & 0xF0) >> 4;
    return std::make_tuple(x, y, z);
}
/**/

// These are where the real opcodes begin:

void SYS_Opcode::opcode(Chip8 &chip8, uint16_t ins)
{
    switch (ins)
    {
    // 00E0 - CLS
    // Clear the display.
    case (0x00E0):
        for (auto &i : chip8.vram)
        {
            i.fill(false);
        }
        break;
    // 00EE - RET
    // Return from a subroutine.
    case (0x00EE):
        chip8.position.pop();
        break;
    }
}

void JMP_Opcode::opcode(Chip8 &chip8, uint16_t ins)
{
    uint16_t location = nnn(ins);
    chip8.position.set(location);
}

void CALL_Opcode::opcode(Chip8 &chip8, uint16_t ins)
{
    uint16_t location = nnn(ins);
    chip8.position.push(location);
}

void SE_Opcode::opcode(Chip8 &chip8, uint16_t ins)
{
    auto [reg, val] = xkk(ins);
    if (chip8.registers[reg] == val)
    {
        chip8.position.step();
    }
}

void SNE_Opcode::opcode(Chip8 &chip8, uint16_t ins)
{
    auto [reg, val] = xkk(ins);
    if (chip8.registers[reg] != val)
    {
        chip8.position.step();
    }
}

void SE_R_Opcode::opcode(Chip8 &chip8, uint16_t ins)
{
    auto [reg1, reg2] = xy0(ins);
    if (chip8.registers[reg1] == chip8.registers[reg2])
    {
        chip8.position.step();
    }
}

void LD_Opcode::opcode(Chip8 &chip8, uint16_t ins)
{
    auto [reg, val] = xkk(ins);
    chip8.registers[reg] = val;
}

void ADD_Opcode::opcode(Chip8 &chip8, uint16_t ins)
{
    auto [reg, val] = xkk(ins);
    chip8.registers[reg] += val;
}

void REG_Opcode::opcode(Chip8 &chip8, uint16_t ins)
{
    auto [reg1_pos, reg2_pos, i] = xyz(ins);
    auto reg1 = std::make_unique<uint8_t>(chip8.registers[reg1_pos]);
    auto reg2 = std::make_unique<uint8_t>(chip8.registers[reg2_pos]);
    auto carry = std::make_unique<uint8_t>(chip8.registers[0xF]);
    switch (i)
    {
    // LD
    case 0:
        *reg1 = *reg2;
        break;
    // OR
    case 1:
        *reg1 |= *reg2;
        break;
    // AND
    case 2:
        *reg1 &= *reg2;
        break;
    // XOR
    case 3:
        *reg1 ^= *reg2;
        break;
    // ADD
    case 4:
    {
        uint16_t result = *reg1 + *reg2;
        *carry = result > UINT8_MAX ? 1 : 0;
        *reg1 += *reg2;
        break;
    }
    // SUB
    case 5:
        *carry = *reg1 > *reg2 ? 1 : 0;
        *reg1 -= *reg2;
        break;
    // SHR
    case 6:
        *carry = *reg1 & 1;
        *reg1 >>= 1;
        break;
    // SUBN
    case 7:
        *carry = *reg2 > *reg1 ? 1 : 0;
        *reg1 -= *reg2;
        break;
    // SHL
    case 0xE:
        *carry = (*reg1 >> 8) & 1;
        *reg1 <<= 1;
        break;
    }
}

void SNE_R_Opcode::opcode(Chip8 &chip8, uint16_t ins)
{
    auto [reg1, reg2] = xy0(ins);
    if (chip8.registers[reg1] != chip8.registers[reg2])
    {
        chip8.position.step();
    }
}

void LD_NNN_Opcode::opcode(Chip8 &chip8, uint16_t ins)
{
    chip8.i = nnn(ins);
}

void JMP_R_Opcode::opcode(Chip8 &chip8, uint16_t ins)
{
    chip8.position.set(chip8.registers[0] + nnn(ins));
}

void RND_Opcode::opcode(Chip8 &chip8, uint16_t ins)
{
    auto [reg, val] = xkk(ins);
    uint8_t r = rand() % 256;
    chip8.registers[reg] = val & r;
}

void DRW_Opcode::opcode(Chip8 &chip8, uint16_t ins)
{
    auto [reg_pos_x, reg_pos_y, bytes] = xyz(ins);
    auto x = chip8.registers[reg_pos_x];
    auto y = chip8.registers[reg_pos_y];
    const auto length = sizeof(uint8_t);
    for (uint8_t i = 0; i < bytes; i++)
    {
        auto byte = chip8.ram[chip8.i + i];
        auto py = (y + i) % DISPLAY_HEIGHT;
        for (uint8_t j = 0; j < length; j++)
        {
            auto px = (x + j) % DISPLAY_WIDTH;
            auto bit = (byte >> (length - j)) & 1;
            chip8.vram[py][px] ^= bit;
        }
    }
}

void SKP_K_Opcode::opcode(Chip8 &chip8, uint16_t ins)
{
    auto [reg_pos, i] = xkk(ins);
    auto reg = std::make_unique<uint8_t>(chip8.registers[reg_pos]);
    switch (i)
    {
    case 0x9E:
        if (chip8.keyboard[*reg])
            chip8.position.step();
        break;
    case 0xA1:
        if (!chip8.keyboard[*reg])
            chip8.position.step();
        break;
    }
}

void LD_T_Opcode::opcode(Chip8 &chip8, uint16_t ins)
{
    auto [reg_pos, i] = xkk(ins);
    auto reg = std::make_unique<uint8_t>(chip8.registers[reg_pos]);
    switch (i)
    {
    case 0x07:
        *reg = chip8.dt;
        break;
    // aaaa ok i will need to create a special state for this...
    case 0x0A:
        break;
    case 0x15:
        chip8.dt = *reg;
        break;
    case 0x18:
        chip8.st = *reg;
        break;
    case 0x1E:
        chip8.i += *reg;
        break;
    case 0x29:
        // TODO: need to implement the numbers i suppose...
        break;
    case 0x33:
        for (int i = 0; i < 3; i++)
        {
            uint8_t val = (*reg >> (8 - 4 * i)) & 1;
            chip8.ram[chip8.i + i] = val;
        }
        break;
    case 0x55:
        for (int i = 0; i <= reg_pos; i++)
        {
            chip8.ram[chip8.i + i] = chip8.registers[i];
        }
        break;
    case 0x65:
        for (int i = 0; i <= reg_pos; i++)
        {
            chip8.registers[i] = chip8.ram[chip8.i + i];
        }
        break;
    }
}

void PositionStack::set(uint16_t pos)
{
    if ((pos < PRG_RAM_OFFSET) || (pos >= RAM_SIZE))
    {
        throw std::runtime_error("Attempting to jump to invalid position.");
    }
    this->stack[this->pos] = pos;
}

void PositionStack::push(uint16_t pos)
{
    this->pos++;
    if (this->pos >= STACK_SIZE)
    {
        throw std::runtime_error("Stack limit reached");
    }
    this->set(pos);
}

uint16_t PositionStack::get()
{
    return this->stack[this->pos];
}

uint16_t PositionStack::pop()
{
    uint16_t ret = this->get();
    this->pos--;
    if (this->pos < 0)
    {
        throw std::runtime_error("Trying to reach stack position in the negatives");
    }
    return ret;
}

void PositionStack::step()
{
    this->set(this->get() + POS_STEP);
}

Chip8::Chip8()
{
    // initialize the opcodes
    // THIS IS THE WORST THING IVE EVER DONE HOLY SHIT
    opcodes[0x0] = std::make_unique<Opcode>(SYS_Opcode());
    opcodes[0x1] = std::make_unique<Opcode>(JMP_Opcode());
    opcodes[0x2] = std::make_unique<Opcode>(CALL_Opcode());
    opcodes[0x3] = std::make_unique<Opcode>(SE_Opcode());
    opcodes[0x4] = std::make_unique<Opcode>(SNE_Opcode());
    opcodes[0x5] = std::make_unique<Opcode>(SE_R_Opcode());
    opcodes[0x6] = std::make_unique<Opcode>(LD_Opcode());
    opcodes[0x7] = std::make_unique<Opcode>(ADD_Opcode());
    opcodes[0x8] = std::make_unique<Opcode>(REG_Opcode());
    opcodes[0x9] = std::make_unique<Opcode>(SNE_R_Opcode());
    opcodes[0xA] = std::make_unique<Opcode>(LD_NNN_Opcode());
    opcodes[0xB] = std::make_unique<Opcode>(JMP_R_Opcode());
    opcodes[0xC] = std::make_unique<Opcode>(RND_Opcode());
    opcodes[0xD] = std::make_unique<Opcode>(DRW_Opcode());
    opcodes[0xE] = std::make_unique<Opcode>(SKP_K_Opcode());
    opcodes[0xF] = std::make_unique<Opcode>(LD_T_Opcode());
    // we need to initialize the RAM with numbers
    for (int i = 0; i < CHIP8_NUMBERS.size(); i++)
    {
        size_t offset = DIGIT_SIZE * i;
        for (int j = 0; j < DIGIT_SIZE; j++)
        {
            ram[offset + j] = CHIP8_NUMBERS[i][j];
        }
    }
    for (auto &i : vram)
    {
        i.fill(false);
    }
    position.set(PRG_RAM_OFFSET);
}

void Chip8::cycle()
{
    uint16_t ins = (ram[position.get()] << 8) |
                   ram[position.get() + 1];
    uint8_t mask = (ram[position.get()] >> 4) & 0xF;
    fmt::print("MASK: {}\n", mask);
    fmt::print("INSTRUCTION: {}\n", ins);
    fmt::print("INSTRUCTION 1: {}\n", ram[position.get() + 1]);
}

void Chip8::load(std::vector<uint8_t> &program)
{
    if (program.size() > (RAM_SIZE - PRG_RAM_OFFSET))
        throw std::runtime_error("Loaded program is too large to fit in memory");
    for (int i = 0; i < program.size(); i++)
    {
        ram[PRG_RAM_OFFSET + i] = program[i];
    }
}