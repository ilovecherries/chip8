#include "chip8.h"
#include <stdexcept>
#include <tuple>
#include <memory>
#include <cstdlib>

// helper functions for getting values out of opcode instructions
/**/
uint16_t nnn(uint16_t ins)
{
    return 0xFFF & ins;
}

std::tuple<uint8_t, uint8_t> xkk(uint16_t ins)
{
    uint8_t x = (ins >> 8) & 0xF;
    uint8_t kk = ins & 0xFF;
    return std::make_tuple(x, kk);
}

std::tuple<uint8_t, uint8_t> xy0(uint16_t ins)
{
    uint8_t x = (ins >> 8) & 0xF;
    uint8_t y = (ins >> 4) & 0xF;
    return std::make_tuple(x, y);
}

std::tuple<uint8_t, uint8_t, uint8_t> xyz(uint16_t ins)
{
    uint8_t x = (ins >> 8) & 0xF;
    uint8_t y = (ins >> 4) & 0xF;
    uint8_t z = ins & 0xF;
    return std::make_tuple(x, y, z);
}
/**/

// These are where the real opcodes begin:

void SYS_opcode(Chip8 &chip8, uint16_t ins)
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

void JMP_opcode(Chip8 &chip8, uint16_t ins)
{
    uint16_t location = nnn(ins);
    chip8.position.set(location);
    chip8.position.back();
}

void CALL_opcode(Chip8 &chip8, uint16_t ins)
{
    uint16_t location = nnn(ins);
    chip8.position.push(location);
    chip8.position.back();
}

void SE_opcode(Chip8 &chip8, uint16_t ins)
{
    auto [reg, val] = xkk(ins);
    if (chip8.registers[reg] == val)
    {
        chip8.position.step();
    }
}

void SNE_opcode(Chip8 &chip8, uint16_t ins)
{
    auto [reg, val] = xkk(ins);
    if (chip8.registers[reg] != val)
    {
        chip8.position.step();
    }
}

void SE_R_opcode(Chip8 &chip8, uint16_t ins)
{
    auto [reg1, reg2] = xy0(ins);
    if (chip8.registers[reg1] == chip8.registers[reg2])
    {
        chip8.position.step();
    }
}

void LD_opcode(Chip8 &chip8, uint16_t ins)
{
    auto [reg, val] = xkk(ins);
    chip8.registers[reg] = val;
}

void ADD_opcode(Chip8 &chip8, uint16_t ins)
{
    auto [reg, val] = xkk(ins);
    chip8.registers[reg] += val;
}

void REG_opcode(Chip8 &chip8, uint16_t ins)
{
    auto [reg1_pos, reg2_pos, i] = xyz(ins);
    auto &reg1 = chip8.registers[reg1_pos];
    auto &reg2 = chip8.registers[reg2_pos];
    auto &carry = chip8.registers[0xF];
    switch (i)
    {
    // LD
    case 0:
        reg1 = reg2;
        break;
    // OR
    case 1:
        reg1 |= reg2;
        break;
    // AND
    case 2:
        reg1 &= reg2;
        break;
    // XOR
    case 3:
        reg1 ^= reg2;
        break;
    // ADD
    case 4:
    {
        uint16_t result = reg1 + reg2;
        carry = result > UINT8_MAX ? 1 : 0;
        reg1 += reg2;
        break;
    }
    // SUB
    case 5:
        carry = reg1 > reg2 ? 1 : 0;
        reg1 -= reg2;
        break;
    // SHR
    case 6:
        carry = reg1 & 1;
        reg1 >>= 1;
        break;
    // SUBN
    case 7:
        carry = reg2 > reg1 ? 1 : 0;
        reg1 -= reg2;
        break;
    // SHL
    case 0xE:
        carry = (reg1 >> 8) & 1;
        reg1 <<= 1;
        break;
    }
}

void SNE_R_opcode(Chip8 &chip8, uint16_t ins)
{
    auto [reg1, reg2] = xy0(ins);
    if (chip8.registers[reg1] != chip8.registers[reg2])
    {
        chip8.position.step();
    }
}

void LD_NNN_opcode(Chip8 &chip8, uint16_t ins)
{
    chip8.i = nnn(ins);
}

void JMP_R_opcode(Chip8 &chip8, uint16_t ins)
{
    chip8.position.set(chip8.registers[0] + nnn(ins));
}

void RND_opcode(Chip8 &chip8, uint16_t ins)
{
    auto [reg, val] = xkk(ins);
    uint8_t r = rand() % 256;
    chip8.registers[reg] = val & r;
}

void DRW_opcode(Chip8 &chip8, uint16_t ins)
{
    auto [reg_pos_x, reg_pos_y, bytes] = xyz(ins);
    auto x = chip8.registers[reg_pos_x];
    auto y = chip8.registers[reg_pos_y];
    chip8.registers[0xF] = 0;
    for (uint8_t i = 0; i < bytes; i++)
    {
        auto byte = chip8.ram[chip8.i + i];
        auto py = (y + i) % DISPLAY_HEIGHT;
        for (int8_t j = 7; j >= 0; j--)
        {
            auto px = (x + 7 - j) % DISPLAY_WIDTH;
            auto bit = (byte >> j) & 1;
            chip8.registers[0xF] |= bit & chip8.vram[py][px];
            chip8.vram[py][px] ^= bit;
        }
    }
}

void SKP_K_opcode(Chip8 &chip8, uint16_t ins)
{
    auto [reg_pos, i] = xkk(ins);
    auto &reg = chip8.registers[reg_pos];
    switch (i)
    {
    case 0x9E:
        if (chip8.keyboard[reg])
            chip8.position.step();
        break;
    case 0xA1:
        if (!chip8.keyboard[reg])
            chip8.position.step();
        break;
    }
}

void LD_T_opcode(Chip8 &chip8, uint16_t ins)
{
    auto [reg_pos, i] = xkk(ins);
    auto &reg = chip8.registers[reg_pos];
    switch (i)
    {
    case 0x07:
        reg = chip8.dt;
        break;
    // aaaa ok i will need to create a special state for this...
    case 0x0A:
        break;
    case 0x15:
        chip8.dt = reg;
        break;
    case 0x18:
        chip8.st = reg;
        break;
    case 0x1E:
        chip8.i += reg;
        break;
    case 0x29:
        chip8.i = reg * DIGIT_SIZE;
        break;
    case 0x33:
        chip8.ram[chip8.i] = (reg / 100) % 10;
        chip8.ram[chip8.i + 1] = (reg / 10) % 10;
        chip8.ram[chip8.i + 2] = reg % 10;
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

void PositionStack::back()
{
    this->set(this->get() - POS_STEP);
}

Chip8::Chip8()
{
    ram.fill(0);
    registers.fill(0);
    keyboard.fill(false);
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
    if (!running)
        return;
    uint16_t ins = (ram[position.get()] << 8) |
                   ram[position.get() + 1];
    uint8_t mask = (ram[position.get()] >> 4) & 0xF;
    try
    {
        switch (mask)
        {
        case 0:
            SYS_opcode(*this, ins);
            break;
        case 1:
            JMP_opcode(*this, ins);
            break;
        case 2:
            CALL_opcode(*this, ins);
            break;
        case 3:
            SE_opcode(*this, ins);
            break;
        case 4:
            SNE_opcode(*this, ins);
            break;
        case 5:
            SE_R_opcode(*this, ins);
            break;
        case 6:
            LD_opcode(*this, ins);
            break;
        case 7:
            ADD_opcode(*this, ins);
            break;
        case 8:
            REG_opcode(*this, ins);
            break;
        case 9:
            SNE_R_opcode(*this, ins);
            break;
        case 0xA:
            LD_NNN_opcode(*this, ins);
            break;
        case 0xB:
            JMP_R_opcode(*this, ins);
            break;
        case 0xC:
            RND_opcode(*this, ins);
            break;
        case 0xD:
            DRW_opcode(*this, ins);
            break;
        case 0xE:
            SKP_K_opcode(*this, ins);
            break;
        case 0xF:
            LD_T_opcode(*this, ins);
            break;
        }
        this->position.step();
    }
    catch (std::runtime_error e)
    {
        running = false;
    }
}

void Chip8::load(std::vector<uint8_t> &program)
{
    if (program.size() > (RAM_SIZE - PRG_RAM_OFFSET))
        throw std::runtime_error("Loaded program is too large to fit in memory");
    for (int i = 0; i < program.size(); i++)
    {
        ram[PRG_RAM_OFFSET + i] = program[i];
    }
    running = true;
}

void Chip8::timer(uint64_t millis)
{
    if (oldMillis)
    {
        uint64_t dif = millis - oldMillis + remMillis;
        auto frames = dif / FRAME_MILLIS;
        remMillis = dif % FRAME_MILLIS;
        if (dt)
            dt = std::max(0UL, dt - frames);
        if (st)
            st = std::max(0UL, st - frames);
    }
    oldMillis = millis;
}