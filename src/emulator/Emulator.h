#ifndef EMULATOR__H
#define EMULATOR__H

#include <array>
#include <vector>

namespace chip8 {

constexpr uint32_t TOTAL_MEMORY_SIZE_BYTES = 0xFFF;
constexpr uint8_t TOTAL_GPR_SIZE_BYTES = 16;
constexpr uint8_t TOTAL_STACK_SIZE_BYTES = 32;
constexpr double TARGET_HZ = 60;
constexpr uint8_t DIGIT_SPRITE_SIZE_BYTES = 5;
constexpr uint8_t SPRITE_DIGIT_AMOUNT = 16;

using SpriteDigit = std::array<uint8_t, DIGIT_SPRITE_SIZE_BYTES>;
constexpr SpriteDigit sprite0 = {0xF0, 0x90, 0x90, 0x90, 0xF0};
constexpr SpriteDigit sprite1 = {0x20, 0x60, 0x20, 0x20, 0x70};
constexpr SpriteDigit sprite2 = {0xF0, 0x10, 0xF0, 0x80, 0xF0};
constexpr SpriteDigit sprite3 = {0xF0, 0x10, 0xF0, 0x10, 0xF0};
constexpr SpriteDigit sprite4 = {0x90, 0x90, 0xF0, 0x10, 0x10};
constexpr SpriteDigit sprite5 = {0xF0, 0x80, 0xF0, 0x10, 0xF0};
constexpr SpriteDigit sprite6 = {0xF0, 0x80, 0xF0, 0x90, 0xF0};
constexpr SpriteDigit sprite7 = {0xF0, 0x10, 0x20, 0x40, 0x40};
constexpr SpriteDigit sprite8 = {0xF0, 0x90, 0xF0, 0x90, 0xF0};
constexpr SpriteDigit sprite9 = {0xF0, 0x90, 0xF0, 0x10, 0xF0};
constexpr SpriteDigit spriteA = {0xF0, 0x90, 0xF0, 0x90, 0x90};
constexpr SpriteDigit spriteB = {0xE0, 0x90, 0xE0, 0x90, 0xE0};
constexpr SpriteDigit spriteC = {0xF0, 0x80, 0x80, 0x80, 0xF0};
constexpr SpriteDigit spriteD = {0xE0, 0x90, 0x90, 0x90, 0xE0};
constexpr SpriteDigit spriteE = {0xF0, 0x80, 0xF0, 0x80, 0xF0};
constexpr SpriteDigit spriteF = {0xF0, 0x80, 0xF0, 0x80, 0x80};
constexpr std::array<SpriteDigit, SPRITE_DIGIT_AMOUNT> spriteDigits = {
    sprite0, sprite1, sprite2, sprite3, sprite4, sprite5, sprite6, sprite7,
    sprite8, sprite9, spriteA, spriteB, spriteC, spriteD, spriteE, spriteF};

using RegisterSize16 = uint16_t;
using RegisterSize8 = uint8_t;

class Emulator {
public:
  // TODO: docs
  Emulator();

  // TODO: docs
  ~Emulator() = default;

  // TODO: docs
  void loadProgram(std::vector<uint8_t> &romBuffer);

  // TODO: docs
  void execute();

private:
  // TODO: docs
  void loadHexDigitSprites();

  // Memory structure for RAM
  std::array<uint8_t, TOTAL_MEMORY_SIZE_BYTES> _memory;

  // TODO: docs
  std::array<uint16_t, TOTAL_STACK_SIZE_BYTES / 2> _stack;

  // Register used to store memory addresses
  // NOTE: only the three LSB nibbles are used
  RegisterSize16 _instrReg;

  // General purpose registers (Vx registers)
  std::array<RegisterSize8, TOTAL_GPR_SIZE_BYTES> _generalPurposeReg;

  // Not used by programs, used as a flag for some instructions
  RegisterSize8 _vfRegister;

  // When non-zero this register is decremented at a rate of 60hz until zero
  RegisterSize8 _delayReg;

  // When non-zero this register is decremented at a rate of 60hz until zero
  RegisterSize8 _soundTimerReg;

  // Program counter register stores the current executing address
  RegisterSize16 _pcReg;

  // Stack pointer register used to point to the topmost level of the stack
  RegisterSize8 _spReg;
};

} // namespace chip8

#endif
