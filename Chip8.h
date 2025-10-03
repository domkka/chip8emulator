#include <array>
#include <cstdint>
#include <string>

class Chip8 {
public:
  static constexpr int MEMORY_SIZE = 4096;
  static constexpr int DISPLAY_WIDTH = 64;
  static constexpr int DISPLAY_HEIGHT = 32;
  static constexpr int STACK_SIZE = 16;
  static constexpr int VARIABLE_REGISTERS = 16;
  static constexpr int KEYS = 16;
  static constexpr std::array<uint8_t, 80> fontset = {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

  Chip8();
  void reset();
  bool loadROM(const std::string &filename);
  void cycle();
  void clearScreen();
  void draw(uint8_t x, uint8_t y, uint8_t n);

  const std::array<uint8_t, DISPLAY_WIDTH * DISPLAY_HEIGHT> &getDisplay() const { return display; }
  std::array<uint8_t, KEYS> &getKeys() { return keypad; }

private:
  std::array<uint8_t, MEMORY_SIZE> memory{};
  std::array<uint8_t, VARIABLE_REGISTERS> V{};
  uint16_t I{};
  uint16_t PC{};
  std::array<uint16_t, STACK_SIZE> stack{};
  uint8_t delay_timer{};
  uint8_t sound_timer{};
  std::array<uint8_t, KEYS> keypad;
  std::array<uint8_t, DISPLAY_WIDTH * DISPLAY_HEIGHT> display{};
};