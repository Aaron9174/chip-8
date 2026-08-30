#ifndef DISASSEMBLER__H
#define DISASSEMBLER__H

/**
 * Disassembles CHIP8 ROMs into readable CHIP8 assembly
 */
#include <cstdint>
#include <iostream>
#include <vector>

namespace chip8 {

// Defines a printable chip8 assembly instruction
struct Instruction {
  /** Default constructor */
  Instruction() = default;
  /** Operations human readable command label */
  std::string _cmdLabel;
  /** The first optional stringified parameter of the instruction */
  std::optional<std::string> _param1;
  /** The second optional stringified parameter of the instruction */
  std::optional<std::string> _param2;
  /** The third optional stringified parameter of the instruction */
  std::optional<std::string> _param3;
  /** The assembly line informative message */
  std::string _msg;
};

// Define default stream insertion operator
std::ostream &operator<<(std::ostream &os, const Instruction &instr);

/**
 * The disassmbler class consumes a byte array read in from a chip8 ROM and
 * converts it into human readable assembly instructions
 */
class Disassembler {
public:
  /** The constructor */
  Disassembler(std::vector<uint8_t> romBuffer);

  /** Default destructor */
  ~Disassembler() = default;

  /**
   * Deconstructs the binary data in the rom buffer into human readable assembly
   * language
   * @param[out] ss - The output stream
   */
  void deconstruct(std::stringstream &ss);

private:
  /**
   * Format a hex string from a 1-2 byte value
   * @param formatter - The formatter stream to use
   * @param value - The value to format
   * @param byteLen - The max byte length of the value
   * @returns a formatted string
   */
  std::string formatHex(std::stringstream &formatter, uint16_t value,
                        uint32_t byteLen);

  /**
   * Format a register string from a 4 byte value
   * @param formatter - The formatter stream to use
   * @param value - The value to format
   * @param nibbleIndex - The nibble index of where the register nibble can be
   * obtained from (based on a hex string)
   * @returns a formatted string
   */
  std::string formatRegister(std::stringstream &formatter, uint32_t regNum,
                             uint32_t nibbleIndex);

  /**
   * Formats a memory address and outputs it to the given stream
   * @param outputStream - The stream to ouput to
   * @param bufferByteIndex - The current byte index relative to the start of
   * memory
   */
  void formatMemoryAddress(std::stringstream &outputStream,
                           uint32_t bufferByteIndex);

  /**
   * Parses a 2 byte instruction code to a set register assembly instruction
   * @param formatter - The formatter to use
   * @param instrRaw - The raw 2 byte instruction
   * @param[out] instr - The instruction to output too
   */
  void parseSetRegisterInstr(std::stringstream &formatter, uint16_t instrRaw,
                             Instruction &instr);

  /**
   * Parses a 2 byte instruction code to a skip register assembly instruction
   * @param formatter - The formatter to use
   * @param instrRaw - The raw 2 byte instruction
   * @param[out] instr - The instruction to output too
   */
  void parseSkipRegisterInstr(std::stringstream &formatter, uint16_t instrRaw,
                              Instruction &instr);

  /**
   * Parses a 2 byte instruction code to an io and timer register assembly
   * instruction
   * @param formatter - The formatter to use
   * @param instrRaw - The raw 2 byte instruction
   * @param[out] instr - The instruction to output too
   */
  void parseIoAndTimerRegisterInstr(std::stringstream &formatter,
                                    uint16_t instrRaw, Instruction &instr);

  // Contains the raw byte buffer representing the chip8 ROM data
  std::vector<uint8_t> _romBuffer;
};

} // namespace chip8

#endif
