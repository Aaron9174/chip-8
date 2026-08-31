#ifndef COMMON__H
#define COMMON__H

#include <cstdint>

namespace chip8 {

// The starting memory address for chip8 programs
constexpr uint16_t PROGRAM_START_MEM_ADDRESS = 0x200;

// Instruction set mask constants
constexpr uint16_t SET_REG_MASK = 0x000F;
constexpr uint16_t SKP_MASK = 0x00FF;
constexpr uint16_t IO_AND_TIMER_MASK = 0x00FF;

// Binary data mask constants
constexpr uint16_t MASK_FF00 = 0xFF00;
constexpr uint16_t MASK_00FF = 0x00FF;
constexpr uint16_t MASK_F000 = 0xF000;
constexpr uint16_t MASK_0F00 = 0x0F00;
constexpr uint16_t MASK_00F0 = 0x00F0;
constexpr uint16_t MASK_000F = 0x000F;
constexpr uint16_t MASK_0FFF = 0x0FFF;
constexpr uint16_t MASK_LSB_BIT = 0x0001;
constexpr uint16_t MASK_MSB_BIT = 0x8000;

/**
 * Nibbles that represent different set register instruction codes
 */
enum class SetRegInstrCode : uint8_t {
  LD_VX_VY = 0x0,  // Set Vx = Vy (0x8xy0)
  OR_VX_VY = 0x1,  // Set Vx = Vx | Vy (0x8xy1)
  AND_VX_VY = 0x2, // Set Vx = Vx & Vy (0x8xy2)
  XOR_VX_VY = 0x3, // Set Vx = Vx XOR Vy (0x8xy3)
  // Set Vx = Vx + Vy ; Set VF = carry (0x8xy4)
  // NOTE: if Vx + Vy > 255 VF is set to 1, otherwise 0
  // Only the lowest 8-bit result is kept
  ADD_VX_VY = 0x4,
  // Set Vx = Vx - Vy ; Set VF = NOT borrow (0x8xy5)
  // NOTE: If Vx > Vy, VF is is set to 1, otherwise 0. Then operation
  // occurs.
  SUB_VX_VY = 0x5,
  // Set Vx = Vx / 2 (0x8xy6)
  // NOTE: If least significant bit of Vx is 1, Then VF is set to 1,
  // otherwise 0. Then operation occurs.
  SHR_VX_VY = 0x6,
  // Set Vx = Vy - Vx, set VF = NOT borrow (0x8xy7)
  // NOTE: If Vy > Vx, VF is set to 1, otherwise 0 . Then operation occurs.
  SUBN_VX_VY = 0x7,

  /** Unused codes [0x8-0xD] */

  // Set Vx = Vx * 2
  // NOTE: If most significant bit of Vx is asserted, VF is set to 1,
  // otherwise zero. Then operation occurs.
  SHL_VX_VY = 0xE
};

/** Bytes that represent different skip instruction codes */
enum class SkpInstrCode {
  SKP_VX = 0x9E, // Skip next instruction if key with the value of Vx is pressed
  SKNP_VX = 0xA1, // Skip next instruction if the key corresponding to the value
                  // Vx is in the up position
};

enum class IoAndTimerCode {
  LD_VX_DT = 0x07, // Set Vx=delay timer value (0xFx07)
  LD_VX_K = 0x0A,  // Wait for key press, then store key value in Vx (0xFx0A)
  LD_DT_VX = 0x15, // Set delay timer to Vx (0xFx15)
  LD_ST_VX = 0x18, // Set sound timer to Vx (0xFx18)
  ADD_I_VX = 0x1E, // Set I = I + Vx
  LD_F_VX = 0x29,  // Set I = location of sprite at digit Vx
  LD_B_VX = 0x33,  // Store BCD representation of Vx in memory locations I, I+1,
                   // and I+2
  LD_I_VX = 0x55,  // Store register V0-Vx in memory starting at location I
  LD_VX_I = 0x65   // Read registers V0-Vx start at memory location I
};

/** The chip-8 full instruction set (none constant byte values are reprsented as
 * range) */
enum class InstructionCode : uint16_t {
  CLS = 0x00E0,           // Clears the display
  RET = 0x00EE,           // Returns from subroutine
  JP_ADDR_START = 0x1000, // Jump to location 0xnnn (0x1nnn)
  JP_ADDR_END = 0x1FFF,
  CALL_ADDR_START = 0x2000, // Call subroutine at address 0xnnn (0x2nnn)
  CALL_ADDR_END = 0x2FFF,
  SE_VX_KK_START = 0x3000, // Skip next instruction if Vx=kk (0x3xkk)
  SE_VX_KK_END = 0x3FFF,
  SNE_VX_KK_START = 0x4000, // Skip next instruction if Vx!=kk (0x4xkk)
  SNE_VX_KK_END = 0x4FFF,
  SE_VX_VY_START = 0x5000, // Skip next instruction if Vx=Vy (0x5xy0)
  SE_VX_VY_END = 0x5FF0,
  LD_VX_KK_START = 0x6000, // Place value in register s.t. Vx=kk (0x6xkk)
  LD_VX_KK_END = 0x6FFF,
  ADD_VX_KK_START = 0x7000, // Adds value to Vx s.t. Vx=Vx+kk (0x7xkk)
  ADD_VX_KK_END = 0x7FFF,
  SET_REG_START = 0x8000, // Various set register commands (used with masking)
  SET_REG_END = 0x8FFE,
  SNE_VX_VY_START = 0x9000, // Skip next instruction if Vx!=Vy (0x9xy0)
  SNE_VX_VY_END = 0x9FF0,
  LD_I_ADDR_START = 0xA000, // Set register I s.t. I=nnn (0xAnnn)
  LD_I_ADDR_END = 0xAFFF,
  JP_V0_ADDR_START = 0xB000, // Jump to location nnn + V0 (0xBnnn)
  JP_V0_ADDR_END = 0xBFFF,
  RND_VX_KK_START = 0xC000, // Set register Vx=Rnd(byte)&kk (0xCxkk)
  RND_VX_KK_END = 0xCFFF,
  DRW_VX_VY_START = 0xD000, // Display n-byte sprite starting at memory location
                            // I at (Vx, Vy), set VF = collision (0xDxyn)
  DRW_VX_VY_END = 0xDFFF,
  SKP_START = 0xE000, // Defines a possible skip instruction (used with masking)
  SKP_END = 0xEFFF,
  IO_AND_TIMER_INSTR_START = 0xF000,
  IO_AND_TIMER_INSTR_END = 0xFFFF,
};

} // namespace chip8

#endif
