#include "Emulator.h"
#include "Common.h"
#include <limits>
#include <random>
#include <stdexcept>

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
    executeSetRegInstr(opcode);
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
    executeSkipRegInstr(opcode);
  } else if (opcode >= InstructionCode::IO_AND_TIMER_INSTR_START &&
             opcode <= InstructionCode::IO_AND_TIMER_INSTR_END) {
    // TODO: implement io and timer instruction behavior
  } else {
    throw std::runtime_error(UNRECOG_OP_CODE_ERR);
  }
}

/****************************************************************************/
/****************************************************************************/
void Emulator::executeSetRegInstr(InstructionCode opcode) {
  const uint16_t opcodeByte = static_cast<uint16_t>(opcode);
  const auto setRegOpcode =
      static_cast<SetRegInstrCode>(opcodeByte & MASK_000F);
  const uint8_t vregXIndex = opcodeByte & MASK_0F00;
  const uint8_t vregYIndex = opcodeByte & MASK_00F0;

  switch (setRegOpcode) {
  case SetRegInstrCode::LD_VX_VY:
    // Set Vx = Vy (0x8xy0)
    _vReg[vregXIndex] = _vReg[vregYIndex];
    break;
  case SetRegInstrCode::OR_VX_VY:
    // Set Vx = Vx | Vy (0x8xy1)
    _vReg[vregXIndex] = _vReg[vregXIndex] | _vReg[vregYIndex];
    break;
  case SetRegInstrCode::AND_VX_VY:
    // Set Vx = Vx & Vy (0x8xy2)
    _vReg[vregXIndex] = _vReg[vregXIndex] & _vReg[vregYIndex];
    break;
  case SetRegInstrCode::XOR_VX_VY:
    // Set Vx = Vx XOR Vy (0x8xy3)
    _vReg[vregXIndex] = _vReg[vregXIndex] ^ _vReg[vregYIndex];
    break;
  case SetRegInstrCode::ADD_VX_VY:
    // Set Vx = Vx + Vy ; Set VF = carry (0x8xy4)
    // NOTE: if Vx + Vy > 255 VF is set to 1, otherwise 0
    // Only the lowest 8-bit result is kept
    if ((_vReg[vregXIndex] + _vReg[vregYIndex]) >
        std::numeric_limits<uint8_t>::max()) {
      _vfRegister = 1;
    } else {
      _vfRegister = 0;
    }
    _vReg[vregXIndex] =
        (_vReg[vregXIndex] + _vReg[vregYIndex]) % sizeof(RegisterSize8);
    break;
  case SetRegInstrCode::SUB_VX_VY:
    // Set Vx = Vx - Vy ; Set VF = NOT borrow (0x8xy5)
    // NOTE: If Vx > Vy, VF is is set to 1, otherwise 0. Then operation
    // occurs.
    if (_vReg[vregXIndex] > _vReg[vregYIndex]) {
      _vfRegister = 1;
    } else {
      _vfRegister = 0;
    }
    _vReg[vregXIndex] = _vReg[vregXIndex] - _vReg[vregYIndex];
    break;
  case SetRegInstrCode::SHR_VX_VY:
    // Set Vx = Vx / 2 (0x8xy6)
    // NOTE: If least significant bit of Vx is 1, Then VF is set to 1,
    // otherwise 0. Then operation occurs.
    if (_vReg[vregXIndex] & MASK_LSB_BIT) {
      _vfRegister = 1;
    } else {
      _vfRegister = 0;
    }
    _vReg[vregXIndex] = _vReg[vregXIndex] / 2;
    break;
  case SetRegInstrCode::SUBN_VX_VY:
    // Set Vx = Vy - Vx, set VF = NOT borrow (0x8xy7)
    // NOTE: If Vy > Vx, VF is set to 1, otherwise 0 . Then operation occurs.
    if (_vReg[vregYIndex] > _vReg[vregXIndex]) {
      _vfRegister = 1;
    } else {
      _vfRegister = 0;
    }
    _vReg[vregXIndex] = _vReg[vregYIndex] - _vReg[vregXIndex];
    break;
  case SetRegInstrCode::SHL_VX_VY:
    // Set Vx = Vx * 2
    // NOTE: If most significant bit of Vx is asserted, VF is set to 1,
    // otherwise zero. Then operation occurs.
    if (_vReg[vregXIndex] & MASK_MSB_BIT) {
      _vfRegister = 1;
    } else {
      _vfRegister = 0;
    }
    _vReg[vregXIndex] = _vReg[vregXIndex] * 2;
    break;
  default:
    throw std::runtime_error(UNRECOG_OP_CODE_ERR);
  }
}

/****************************************************************************/
/****************************************************************************/
void Emulator::executeSkipRegInstr(InstructionCode opcode) {
  const uint16_t opcodeByte = static_cast<uint16_t>(opcode);
  const auto skipInstrOpcode =
      static_cast<SkpInstrCode>(opcodeByte & MASK_00FF);

  switch (skipInstrOpcode) {
  case SkpInstrCode::SKP_VX:
    // Skip next instruction if key with the value of Vx is pressed (0xEx9E)
    // TODO: this has to be done with key interrupts
    break;
  case SkpInstrCode::SKNP_VX:
    // Skip next instruction if the key corresponding to the value
    // Vx is in the up position
    // TODO: this has to be done with key interrupts
    break;
  default:
    throw std::runtime_error(UNRECOG_OP_CODE_ERR);
  }
}

/****************************************************************************/
/****************************************************************************/
void Emulator::executeIoTimerRegInstr(InstructionCode opcode) {
  const uint16_t opcodeByte = static_cast<uint16_t>(opcode);
  const auto ioTimerOpcode =
      static_cast<IoAndTimerCode>(opcodeByte & MASK_00FF);
  const uint8_t vregXIndex = opcodeByte & MASK_0F00;

  switch (ioTimerOpcode) {
  case IoAndTimerCode::LD_VX_DT:
    // Set Vx=delay timer value (0xFx07)
    _vReg[vregXIndex] = _delayReg;
    break;
  case IoAndTimerCode::LD_VX_K:
    // Wait for key press, then store key value in Vx (0xFx0A)
    // TODO: implement interrupts
    break;
  case IoAndTimerCode::LD_DT_VX:
    // Set delay timer to Vx (0xFx15)
    _delayReg = _vReg[vregXIndex];
    break;
  case IoAndTimerCode::LD_ST_VX:
    // Set sound timer to Vx (0xFx18)
    _soundTimerReg = _vReg[vregXIndex];
    break;
  case IoAndTimerCode::ADD_I_VX:
    // Set I = I + Vx (0xFx1E)
    _iReg = _iReg + _vReg[vregXIndex];
    break;
  case IoAndTimerCode::LD_F_VX:
    // Set I = location of sprite at digit Vx (0xFx29)
    _iReg = getSpriteDigitAddress(_vReg[vregXIndex]);
    break;
  case IoAndTimerCode::LD_B_VX:
    executeBcdInstr(vregXIndex);
    break;
  case IoAndTimerCode::LD_I_VX:
    for (uint8_t i = 0; i <= vregXIndex; i++) {
      _memory[_iReg + i] = _vReg[i];
    }
    break;
  case IoAndTimerCode::LD_VX_I:
    for (uint8_t i = 0; i <= vregXIndex; i++) {
      _vReg[i] = _memory[_iReg + i];
    }
    break;
  default:
    throw std::runtime_error(UNRECOG_OP_CODE_ERR);
  }
}

/****************************************************************************/
/****************************************************************************/
void Emulator::executeBcdInstr(uint8_t vregXIndex) {
  // Store BCD representation of Vx in memory locations I, I+1,
  // and I+2 (0xFx33)
  RegisterSize8 vregVal = _vReg[vregXIndex];
  uint8_t hundredsPlace = vregVal / 100;
  uint8_t tensPlace = (_vReg[vregXIndex] / 10) % 10;
  uint8_t onesPlace = _vReg[vregXIndex] % 10;
  _memory[_iReg] = hundredsPlace;
  _memory[_iReg + 1] = tensPlace;
  _memory[_iReg + 2] = onesPlace;
}

/****************************************************************************/
/****************************************************************************/
RegisterSize16 Emulator::getSpriteDigitAddress(uint8_t spriteValue) {
  return DIGIT_SPRITE_SIZE_BYTES * spriteValue;
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
