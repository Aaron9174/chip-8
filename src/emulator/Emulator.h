#ifndef EMULATOR__H
#define EMULATOR__H

#include "Common.h"
#include "Display.h"
#include <array>
#include <cstdint>
#include <sstream>
#include <vector>

namespace chip8 {

constexpr uint32_t TOTAL_MEMORY_SIZE_BYTES = 0xFFF;
constexpr uint8_t TOTAL_GPR_SIZE_BYTES = 16;
constexpr uint8_t TOTAL_STACK_SIZE_BYTES = 32;
constexpr double TARGET_HZ = 60;
constexpr uint8_t DIGIT_SPRITE_SIZE_BYTES = 5;
constexpr uint8_t SPRITE_DIGIT_AMOUNT = 16;

constexpr const char *UNRECOG_OP_CODE_ERR = "[Error] Unrecognized opcode";

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

enum class InputState : uint8_t {
  NONE = 0,
  WAITING = 1,
  INPUT_RECIEVED = 2,
};

class Emulator {
public:
  /** Constructor */
  Emulator();

  /** Destructor */
  ~Emulator() = default;

  /**
   * Load the ROM program into memory
   * @param romBuffer - The ROM buffer of bytes to load
   */
  void loadProgram(std::vector<uint8_t> &romBuffer);

  /**
   * Starts the emulator
   * NOTE: runs the core loop at ~700Hz but display and sound/delay registers at
   * 60Hz
   */
  void start();

  /**
   * Retrieves the debug stream
   * @return the debug stream containing debug information about register states
   */
  std::stringstream &getDebugStream();

private:
  /**
   * Loads all 16 the default display hex number byte data at the start of
   * memory
   */
  void loadHexDigitSprites();

  /**
   * Fetches the instruction opcode (two-bytes) and increments the PC counter by
   * 2
   * @returns the 2-byte instruction code
   */
  InstructionCode fetch();

  /**
   * Executes the instruction opcode
   * @param opcode - The opcode to execute
   * @throws if opcode is unrecognized
   */
  void execute(InstructionCode opcode);

  /**
   * Draw a sprite starting at memory location I at display location (Vx, Vy)
   * NOTE: Also sets VF to 1 if collision occurred
   */
  void drawSprite(uint16_t opcodeByte);

  /**
   * Executes a set register instruction (opcode between 0x8000-0x8FFE)
   * @param opcode - The set register opcode
   * @throws if opcode is unrecognized
   */
  void executeSetRegInstr(InstructionCode opcode);

  /**
   * Executes a skip register instruction (opcode between 0xE000-0xEFFF)
   * @param opcode - The skip register instruction op code
   * @throws if opcode is unrecognized
   */
  void executeSkipRegInstr(InstructionCode opcode);

  /**
   * Executes a IO timer register instruction
   * @param opcode - The IO timer register opcode
   * @throws if opcode is unrecognized
   */
  void executeIoTimerRegInstr(InstructionCode opcode);

  /**
   * Takes the v register index, and then updates memory of at I, I+1, and I+2
   * with the decimal hundreds, tens, and ones place respectively
   * @param vregXIndex - The v register index to use
   */
  void executeBcdInstr(uint8_t vregXIndex);

  /**
   * Given a sprite digit value (0x0 - 0xF) and the follow:
   * - Sprite digits are stored at the beginning of memory starting 0x0
   * - Sprites digits are 5 bytes long
   * @returns the address of the given value
   */
  RegisterSize16 getSpriteDigitAddress(uint8_t spriteValue);

  /** Generates a number using mt19937 */
  uint8_t generateRandomNumber();

  /** Streams formatted register data to the debug stream */
  void registerToDebugStream();

  // Memory structure for RAM
  std::array<uint8_t, TOTAL_MEMORY_SIZE_BYTES> _memory;

  /**
   * The program stack, used for storing addresses for the interpreter to
   * return to when a subroutine is over
   */
  std::array<uint16_t, TOTAL_STACK_SIZE_BYTES / 2> _stack;

  // Register used to store memory addresses
  // NOTE: only the three LSB nibbles are used
  RegisterSize16 _iReg;

  // General purpose registers (Vx registers)
  std::array<RegisterSize8, TOTAL_GPR_SIZE_BYTES> _vReg;

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

  // The display object used to render the display buffer
  Display _display;

  // 1D array representing the monochrome display
  std::array<RegisterSize8, DISPLAY_WIDTH * DISPLAY_HEIGHT> _displayBuffer;

  // Represents the state of keys, if asserted key is being pressed, otherwise
  // key is unpressed
  KeypadType _keypad;

  /**
   * Determines what input state the emulator is in
   *    NONE - No input has been requested
   *    WAITING - Waiting on input from the user
   *    INPUT_RECIEVED - Input has been received
   */
  InputState _inputState;

  // Stores the last recieved input from the user (0x0-0xF)
  uint8_t _lastRecievedInput;

  // String stream used to format debug data
  std::stringstream _debugStream;
};

} // namespace chip8

#endif
