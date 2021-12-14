#include "chip8.h"
#include <stdexcept>
#include <tuple>
#include <memory>

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
        chip8.vram.fill(0);
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
    {
        int16_t result = *reg1 - *reg2;
        *carry = result < 0 ? 1 : 0;
        *reg1 -= *reg2;
        break;
    }
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