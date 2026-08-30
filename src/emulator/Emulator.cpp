#include "Emulator.h"
#include "Common.h"
#include "emulator.h"

namespace chip8 {

/****************************************************************************/
/****************************************************************************/
Emulator::Emulator()
    : _memory(), _stack{}, _instrReg(0), _generalPurposeReg{}, _vfRegister(0),
      _delayReg(0), _soundTimerReg(0), _pcReg(PROGRAM_START_MEM_ADDRESS),
      _spReg(0) {
  loadHexDigitSprites();
}

/****************************************************************************/
/****************************************************************************/
void Emulator::loadHexDigitSprites() {
  uint32_t memoryByteIndex = 0;
  for (auto spriteDigit : spriteDigits) {
    for (auto byte : spriteDigit) {
      _memory[memoryByteIndex++] = byte;
    }
  }
}

/****************************************************************************/
/****************************************************************************/
void Emulator::loadProgram(std::vector<uint8_t> &romBuffer) {}

/****************************************************************************/
/****************************************************************************/
void Emulator::execute() {
  // TODO: implement execution loop
}

} // namespace chip8
