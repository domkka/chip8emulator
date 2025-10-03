#include "Chip8.h"
#include <ctime>
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
  prevKeypad.fill(0);
  display.fill(0);
  I = 0;
  PC = 0x200;
  sp = 0;
  delay_timer = 0;
  sound_timer = 0;
  for (size_t i = 0; i < fontset.size(); ++i) {
    memory[0x50 + i] = fontset[i];
  }
  std::random_device rd;
  rng.seed(rd());
}

void Chip8::updateTimers() {
  if (delay_timer > 0) {
    delay_timer--;
  }
  if (sound_timer > 0) {
    sound_timer;
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

bool Chip8::isKeyPressed(uint8_t key) {
  return keypad[key];
}

void Chip8::setKey(uint8_t key, bool pressed) {
  keypad[key] = pressed;
  std::cout << "Chip-8 key 0x" << std::hex << (int)key
            << " is now " << (pressed ? "pressed" : "released") << std::endl;
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
    switch (opcode & 0x00FF) {
    case 0x00E0:
      clearScreen();
      break;
    case 0x00EE:
      PC = stack[--sp];
      break;
    }
    break;
  case 0x1000:
    PC = nnn;
    break;
  case 0x2000:
    stack[sp] = PC;
    sp++;
    PC = nnn;
    break;
  case 0x3000:
    if (V[x] == nn) {
      PC += 2;
    }
    break;
  case 0x4000:
    if (V[x] != nn) {
      PC += 2;
    }
    break;
  case 0x5000:
    if (V[x] == V[y]) {
      PC += 2;
    }
    break;
  case 0x6000:
    V[x] = nn;
    break;
  case 0x7000:
    V[x] += nn;
    break;
  case 0x9000:
    if (V[x] != V[y]) {
      PC += 2;
    }
    break;
  case 0xA000:
    I = nnn;
    break;
  case 0xB000:
    PC = nnn + V[0];
    break;
  case 0xC000: {
    std::uniform_int_distribution<uint8_t> dist(0, 255);
    V[x] = dist(rng) & nn;
    break;
  }
  case 0xD000:
    draw(V[x], V[y], n);
    break;
  case 0xE000:
    switch (opcode & 0x00FF) {
    case 0x9E: {
      if (isKeyPressed(V[x])) {
        PC += 2;
      }
      break;
    }
    case 0xA1: {
      if (!isKeyPressed(V[x])) {
        PC += 2;
      }
      break;
    }
    }
    break;
  case 0xF000:
    switch (opcode & 0x00FF) {
    case 0x07:
      V[x] = delay_timer;
      break;
    case 0x15:
      delay_timer = V[x];
      break;
    case 0x18:
      sound_timer = V[x];
      break;
    case 0x1E:
      I += V[x];
      break;
    case 0x0A: {
      bool keyReleased = false;
      for (uint8_t k = 0; k < 16; ++k) {
        if (prevKeypad[k] && !keypad[k]) {
          V[x] = k;
          keyReleased = true;
          break;
        }
      }
      if (!keyReleased) {
        PC -= 2;
      }
      for (uint8_t k = 0; k < 16; ++k) {
        prevKeypad[k] = keypad[k];
      }
      break;
    }
    case 0x29:
      I = 0x50 + V[x] * 5;
      break;
    case 0x33: {
      uint8_t value = V[x];
      memory[I] = value / 100;
      memory[I + 1] = (value / 10) % 10;
      memory[I + 2] = value % 10;
      break;
    }
    case 0x55: {
      for (uint8_t i = 0; i <= x; i++) {
        memory[I] = V[i];
        I++;
      }
      break;
    }
    case 0x65: {
      for (uint8_t i = 0; i <= x; i++) {
        V[i] = memory[I];
        I++;
      }
      break;
    }
    }
    break;
  }

  switch (opcode & 0xF00F) {
  case 0x8000:
    V[x] = V[y];
    break;
  case 0x8001:
    V[x] |= V[y];
    V[0xF] = 0;
    break;
  case 0x8002:
    V[x] &= V[y];
    V[0xF] = 0;
    break;
  case 0x8003:
    V[x] ^= V[y];
    V[0xF] = 0;
    break;
  case 0x8004: {
    uint16_t sum = V[x] + V[y];
    V[x] += V[y];
    V[0xF] = (sum > 0xFF) ? 1 : 0;
    break;
  }
  case 0x8005: {
    uint8_t vx = V[x];
    V[x] -= V[y];
    V[0xF] = (vx >= V[y]) ? 1 : 0;
    break;
  }
  case 0x8006: {
    uint8_t carry = V[y] & 0x1;
    V[x] = V[y] >> 1;
    V[0xF] = carry;
    break;
  }
  case 0x8007:
    V[x] = V[y] - V[x];
    V[0xF] = (V[y] >= V[x]) ? 1 : 0;
    break;
  case 0x800E: {
    uint8_t carry = (V[y] & 0x80) >> 7;
    V[x] = V[y] << 1;
    V[0xF] = carry;
    break;
  }
  }
}