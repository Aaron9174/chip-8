#include "Emulator.h"
#include "Common.h"
#include <limits>
#include <random>

namespace chip8 {

/****************************************************************************/
/****************************************************************************/
Emulator::Emulator()
    : _memory(), _stack{}, _iReg(0), _vReg{}, _vfRegister(0), _delayReg(0),
      _soundTimerReg(0), _pcReg(PROGRAM_START_MEM_ADDRESS), _spReg(0) {
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
void Emulator::loadProgram(std::vector<uint8_t> &romBuffer) {
  uint32_t memoryByteIndex = PROGRAM_START_MEM_ADDRESS;
  for (auto romByte : romBuffer) {
    _memory[memoryByteIndex++] = romByte;
  }
}

/****************************************************************************/
/****************************************************************************/
void Emulator::start() {
  while (true) {
    // TODO: fetch command
    // TODO: execute command (catch errors, report, and break out)
  }
}

/****************************************************************************/
/****************************************************************************/
InstructionCode Emulator::fetch() {
  uint8_t upperByte = _memory[_pcReg];
  uint8_t lowerByte = _memory[_pcReg];
  _pcReg += 2;
  return static_cast<InstructionCode>((upperByte << 8) | lowerByte);
}

/****************************************************************************/
/****************************************************************************/
void Emulator::execute(InstructionCode opcode) {
  const uint16_t opcodeByte = static_cast<uint16_t>(opcode);

  if (opcode == InstructionCode::CLS) {
    // TODO: implement this with display work
  } else if (opcode == InstructionCode::RET) {
    // Interpreter sets the program counter to the top of the stack and the
    // subtracts one from the stack pointer ()
    _pcReg = _stack[_spReg];
    _spReg--;
  } else if (opcode >= InstructionCode::JP_ADDR_START &&
             opcode <= InstructionCode::JP_ADDR_END) {
    // Interpreter sets the program counter to address 0xnnn (0x1nnn)

    const RegisterSize16 jmpAddr = opcodeByte & MASK_0FFF;
    // Infinite loop, throw error
    if (_pcReg == jmpAddr) {
      throw std::runtime_error("[Error] Infinite loop detected.");
    }
    _pcReg = jmpAddr;
  } else if (opcode >= InstructionCode::CALL_ADDR_START &&
             opcode <= InstructionCode::CALL_ADDR_END) {
    const RegisterSize16 callAddr = opcodeByte & MASK_0FFF;
    // Increments stack pointer by 1, puts the current PC on the stack, and then
    // updates PC to address nnn (0x2nnn)
    _spReg++;
    _stack[_spReg] = _pcReg;
    _pcReg = callAddr;
  } else if (opcode >= InstructionCode::SE_VX_KK_START &&
             opcode <= InstructionCode::SE_VX_KK_END) {
    // Skip next instruction (update PC by 2 bytes) if Vx == kk (0x3xkk)
    const uint8_t vregIndex = opcodeByte & MASK_0F00;
    const uint16_t data = opcodeByte & MASK_00FF;
    if (_vReg[vregIndex] == data) {
      _pcReg += 2;
    }
  } else if (opcode >= InstructionCode::SNE_VX_KK_START &&
             opcode <= InstructionCode::SNE_VX_KK_END) {
    // Skip next instruction (update PC by 2 bytes) if Vx != kk (0x4xkk)
    const uint8_t vregIndex = opcodeByte & MASK_0F00;
    const uint16_t data = opcodeByte & MASK_00FF;
    if (_vReg[vregIndex] != data) {
      _pcReg += 2;
    }
  } else if (opcode >= InstructionCode::SE_VX_VY_START &&
             opcode <= InstructionCode::SE_VX_VY_END) {
    // Skip next instruction (update PC by 2 bytes) if Vx == Vy (0x5xy0)
    const uint8_t vregIndex1 = opcodeByte & MASK_0F00;
    const uint8_t vregIndex2 = opcodeByte & MASK_00F0;
    if (_vReg[vregIndex1] == _vReg[vregIndex2]) {
      _pcReg += 2;
    }
  } else if (opcode >= InstructionCode::LD_VX_KK_START &&
             opcode <= InstructionCode::LD_VX_KK_END) {
    // Puts the value kk in register Vx s.t. Vx=kk (0x6xkk)
    const uint8_t vregIndex = opcodeByte & MASK_0F00;
    const RegisterSize8 data = opcodeByte & MASK_00FF;
    _vReg[vregIndex] = data;
  } else if (opcode >= InstructionCode::ADD_VX_KK_START &&
             opcode <= InstructionCode::ADD_VX_KK_END) {
    // Adds value to Vx s.t. Vx=Vx+kk (0x7xkk)
    const uint8_t vregIndex = opcodeByte & MASK_0F00;
    const RegisterSize8 data = opcodeByte & MASK_00FF;
    // NOTE: wrap if greater than the register size
    _vReg[vregIndex] = (_vReg[vregIndex] + data) % sizeof(RegisterSize8);
  } else if (opcode >= InstructionCode::SET_REG_START &&
             opcode <= InstructionCode::SET_REG_END) {
    // TODO: implement set register behavior
  } else if (opcode >= InstructionCode::SNE_VX_VY_START &&
             opcode <= InstructionCode::SNE_VX_VY_END) {
    // Skip next instruction if Vx!=Vy (0x9xy0)
    const uint8_t vregIndex1 = opcodeByte & MASK_0F00;
    const uint8_t vregIndex2 = opcodeByte & MASK_00F0;
    if (_vReg[vregIndex1] != _vReg[vregIndex2]) {
      _pcReg += 2;
    }
  } else if (opcode >= InstructionCode::LD_I_ADDR_START &&
             opcode <= InstructionCode::LD_I_ADDR_END) {
    // Set register I s.t. I=nnn (0xAnnn)
    const RegisterSize16 ldAddr = opcodeByte & MASK_0FFF;
    _iReg = ldAddr;
  } else if (opcode >= InstructionCode::JP_V0_ADDR_START &&
             opcode <= InstructionCode::JP_V0_ADDR_END) {
    // Jump to location nnn + V0 (0xBnnn)
    const RegisterSize16 jmpAddr = opcodeByte & MASK_0FFF;
    _pcReg = _vReg[0] + jmpAddr;
  } else if (opcode >= InstructionCode::RND_VX_KK_START &&
             opcode <= InstructionCode::RND_VX_KK_END) {
    // Set register Vx= Rnd(byte) AND kk (0xCxkk)
    uint8_t randomNum = generateRandomNumber();
    uint8_t data = opcodeByte & MASK_00FF;
    uint8_t vregIndex = opcodeByte & MASK_0F00;
    _vReg[vregIndex] = randomNum & data;
  } else if (opcode >= InstructionCode::DRW_VX_VY_START &&
             opcode <= InstructionCode::DRW_VX_VY_END) {
    // TODO: implement with display logic
  } else if (opcode >= InstructionCode::SKP_START &&
             opcode <= InstructionCode::SKP_END) {
    // TODO: implement skip instruction behavior
  } else if (opcode >= InstructionCode::IO_AND_TIMER_INSTR_START &&
             opcode <= InstructionCode::IO_AND_TIMER_INSTR_END) {
    // TODO: implement io and timer instruction behavior
  } else {
    throw std::runtime_error("[Error] Unrecognized opcode");
  }
}

/****************************************************************************/
/****************************************************************************/
uint8_t Emulator::generateRandomNumber() {
  std::random_device rd;
  std::mt19937 gen(rd());
  const uint8_t minRandomSize = 0;
  const uint8_t maxRandomSize = std::numeric_limits<uint8_t>::max();
  std::uniform_int_distribution<int> dist(minRandomSize, maxRandomSize);
  return static_cast<uint8_t>(dist((gen)));
}

} // namespace chip8
