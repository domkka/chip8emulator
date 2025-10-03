#include "Chip8.h"
#include <fstream>
#include <iostream>

Chip8::Chip8() {
  reset();
}

void Chip8::reset() {
  memory.fill(0);
  V.fill(0);
  stack.fill(0);
  keypad.fill(0);
  display.fill(0);
  I = 0;
  PC = 0x200;
  delay_timer = 0;
  sound_timer = 0;
  for (size_t i = 0; i < fontset.size(); ++i) {
    memory[0x50 + i] = fontset[i];
  }
}

bool Chip8::loadROM(const std::string &filename) {
  std::ifstream file(filename, std::ios::binary | std::ios::ate);

  if (!file) {
    std::cerr << "Failed to open ROM: " << filename << std::endl;
    return false;
  }

  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  if (size > (MEMORY_SIZE - 0x200)) {
    std::cerr << "ROM too large" << std::endl;
    return false;
  }

  file.read(reinterpret_cast<char *>(&memory[0x200]), size);
  return true;
}

void Chip8::clearScreen() {
  display.fill(0);
}

void Chip8::draw(uint8_t x, uint8_t y, uint8_t n) {
  x &= 63;
  y &= 31;
  V[0xF] = 0;
  for (int row = 0; row < n; row++) {

    if (y + row >= DISPLAY_HEIGHT) {
      break;
    }
    uint8_t sprite_byte = memory[I + row];

    for (int col = 0; col < 8; col++) {
      if (x + col >= DISPLAY_WIDTH) {
        break;
      }
      if ((sprite_byte & (0x80 >> col)) != 0) {
        int px = (x + col);
        int py = (y + row);
        int index = px + (py * DISPLAY_WIDTH);

        if (display[index] == 1) {
          V[0xF] = 1;
        }

        display[index] ^= 1;
      }
    }
  }
}

void Chip8::cycle() {
  uint16_t opcode = (memory[PC] << 8) | memory[PC + 1];
  PC += 2;

  uint16_t nnn = opcode & 0x0FFF;
  uint8_t nn = opcode & 0x00FF;
  uint8_t n = opcode & 0x000F;
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;

  switch (opcode & 0xF000) {
  case 0x0000:
    clearScreen();
    break;
  case 0x1000:
    PC = nnn;
    break;
  case 0x6000:
    V[x] = nn;
    break;
  case 0x7000:
    V[x] += nn;
    break;
  case 0xA000:
    I = nnn;
    break;
  case 0xD000:
    draw(V[x], V[y], n);
    break;
  default:
    std::cout << "Unkown opcode: " << std::hex << opcode << std::endl;
    break;
  }
}