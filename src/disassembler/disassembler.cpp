#include "Disassembler.h"
#include "Common.h"
#include <iomanip>
#include <iostream>
#include <sstream>

namespace chip8 {

/****************************************************************************/
/****************************************************************************/
std::ostream &operator<<(std::ostream &os, const Instruction &instr) {
  const std::string PARAM_DELIMITER = ", ";
  const std::string MSG_DELIMITER = " ; ";
  const std::string OPCODE_DELIMITER = " ";
  os << instr._cmdLabel;
  if (instr._param1 && instr._param2 && instr._param3) {
    os << OPCODE_DELIMITER << *instr._param1 << PARAM_DELIMITER
       << *instr._param2 << PARAM_DELIMITER << *instr._param3;
  } else if (instr._param1 && instr._param2) {
    os << OPCODE_DELIMITER << *instr._param1 << PARAM_DELIMITER
       << *instr._param2;
  } else if (instr._param1) {
    os << OPCODE_DELIMITER << *instr._param1;
  }

  os << MSG_DELIMITER << instr._msg << "\n";
  return os;
}

/****************************************************************************/
/****************************************************************************/
Disassembler::Disassembler(std::vector<uint8_t> romBuffer)
    : _romBuffer(std::move(romBuffer)) {}

/****************************************************************************/
/****************************************************************************/
void Disassembler::deconstruct(std::stringstream &ss) {
  uint32_t bufferIndex = 0;

  while (bufferIndex < _romBuffer.size()) {
    // Big endian (most significant byte first)
    const uint8_t upperByte = _romBuffer[bufferIndex];
    const uint8_t lowerByte = _romBuffer[bufferIndex + 1];
    const uint16_t instrRaw = (upperByte << 8) | lowerByte;
    const InstructionCode instrCode = static_cast<InstructionCode>(instrRaw);

    Instruction instr;
    std::stringstream formatter;
    std::cout << "instrRaw: " << instrRaw << std::endl;
    printf("instrRaw: %d", instrRaw);
    printf("JP[%02X, %02X]",
           static_cast<uint16_t>(InstructionCode::JP_ADDR_START),
           static_cast<uint16_t>(InstructionCode::JP_ADDR_END));

    if (instrCode == InstructionCode::CLS) {
      instr._cmdLabel = "CLS";
      instr._msg = "Clears the display";
    } else if (instrCode == InstructionCode::RET) {
      instr._cmdLabel = "RET";
      instr._msg = "Returns from subroutine";
    } else if (instrCode >= InstructionCode::JP_ADDR_START &&
               instrCode <= InstructionCode::JP_ADDR_END) {
      instr._cmdLabel = "JP";
      std::cout << "JP!" << std::endl;
      instr._msg = "Jump to location 0xnnn (0x1nnn)";
      instr._param1 = formatHex(formatter, instrRaw & MASK_0FFF, 2);
    } else if (instrCode >= InstructionCode::CALL_ADDR_START &&
               instrCode <= InstructionCode::CALL_ADDR_END) {
      instr._cmdLabel = "CALL";
      instr._msg = "Call subroutine at address 0xnnn  (0x2nnn)";
      instr._param1 = formatHex(formatter, instrRaw & MASK_0FFF, 2);
    } else if (instrCode >= InstructionCode::SE_VX_KK_START &&
               instrCode <= InstructionCode::SE_VX_KK_END) {
      instr._cmdLabel = "SE";
      instr._msg = "Skip next instruction if Vx=kk (0x3xkk)";
      instr._param1 = formatRegister(formatter, instrRaw & MASK_0F00, 2);
      instr._param2 = formatHex(formatter, instrRaw & MASK_00FF, 2);
    } else if (instrCode >= InstructionCode::SNE_VX_KK_START &&
               instrCode <= InstructionCode::SNE_VX_KK_END) {
      instr._cmdLabel = "SNE";
      instr._msg = "Skip next instruction if Vx!=kk (0x3xkk)";
      instr._param1 = formatRegister(formatter, instrRaw & MASK_0F00, 2);
      instr._param2 = formatHex(formatter, instrRaw & MASK_00FF, 1);
    } else if (instrCode >= InstructionCode::SE_VX_VY_START &&
               instrCode <= InstructionCode::SE_VX_VY_END) {
      instr._cmdLabel = "SE";
      instr._msg = "Skip next instruction if Vx=Vy (0x5xy0)";
      instr._param1 = formatRegister(formatter, instrRaw & MASK_0F00, 2);
      instr._param2 = formatRegister(formatter, instrRaw & MASK_00F0, 1);
    } else if (instrCode >= InstructionCode::LD_VX_KK_START &&
               instrCode <= InstructionCode::LD_VX_KK_END) {
      instr._cmdLabel = "LD";
      instr._msg = "Place value in register such that Vx=kk (0x6xkk)";
      instr._param1 = formatRegister(formatter, instrRaw & MASK_0F00, 2);
      instr._param2 = formatHex(formatter, instrRaw & MASK_00FF, 1);
    } else if (instrCode >= InstructionCode::ADD_VX_KK_START &&
               instrCode <= InstructionCode::ADD_VX_KK_END) {
      instr._cmdLabel = "ADD";
      instr._msg = "Adds value to Vx such that Vx=Vx+kk (0x7xkk)";
      instr._param1 = formatRegister(formatter, instrRaw & MASK_0F00, 2);
      instr._param2 = formatHex(formatter, instrRaw & MASK_00FF, 1);
    } else if (instrCode >= InstructionCode::SET_REG_START &&
               instrCode <= InstructionCode::SET_REG_END) {
      parseSetRegisterInstr(formatter, instrRaw, instr);
    } else if (instrCode >= InstructionCode::SNE_VX_VY_START &&
               instrCode <= InstructionCode::SNE_VX_VY_END) {
      instr._cmdLabel = "SNE";
      instr._msg = "Skip next instruction if Vx!=Vy (0x9xy0)";
      instr._param1 = formatRegister(formatter, instrRaw & MASK_0F00, 2);
      instr._param2 = formatRegister(formatter, instrRaw & MASK_00F0, 1);
    } else if (instrCode >= InstructionCode::LD_I_ADDR_START &&
               instrCode <= InstructionCode::LD_I_ADDR_END) {
      instr._cmdLabel = "LD";
      instr._msg = "Set register I such that I=nnn (0xAnnn)";
      instr._param1 = formatHex(formatter, instrRaw & MASK_0FFF, 2);
    } else if (instrCode >= InstructionCode::JP_V0_ADDR_START &&
               instrCode <= InstructionCode::JP_V0_ADDR_END) {
      instr._cmdLabel = "JP";
      instr._msg = "Jump to location nnn + V0 (0xBnnn)";
      instr._param1 = formatHex(formatter, instrRaw & MASK_0FFF, 2);
    } else if (instrCode >= InstructionCode::RND_VX_KK_START &&
               instrCode <= InstructionCode::RND_VX_KK_END) {
      instr._cmdLabel = "RND";
      instr._msg = "Set register Vx=Rnd(byte)&kk (0xCxkk)";
      instr._param1 = formatRegister(formatter, instrRaw & MASK_0F00, 2);
      instr._param2 = formatHex(formatter, instrRaw & MASK_00FF, 1);
    } else if (instrCode >= InstructionCode::DRW_VX_VY_START &&
               instrCode <= InstructionCode::DRW_VX_VY_END) {
      instr._cmdLabel = "DRW";
      instr._msg = "Display n-byte sprite starting at memory location I at "
                   "(Vx, Vy) (0xDxyn)";
      instr._param1 = formatRegister(formatter, instrRaw & MASK_0F00, 2);
      instr._param2 = formatRegister(formatter, instrRaw & MASK_00F0, 1);
      instr._param3 = formatHex(formatter, instrRaw & MASK_000F, 1);
    } else if (instrCode >= InstructionCode::SKP_START &&
               instrCode <= InstructionCode::SKP_END) {
      parseSkipRegisterInstr(formatter, instrRaw, instr);
    } else if (instrCode >= InstructionCode::IO_AND_TIMER_INSTR_START &&
               instrCode <= InstructionCode::IO_AND_TIMER_INSTR_END) {
      parseIoAndTimerRegisterInstr(formatter, instrRaw, instr);
    } else {
      // NOTE: This is not a real instruction, but utilized for printing generic
      // data to the disassemble file
      instr._cmdLabel = "DATA";
      instr._msg = "Generic data";
      instr._param1 = formatHex(formatter, instrRaw, 2);
    }

    formatMemoryAddress(ss, bufferIndex);
    ss << instr;
    bufferIndex += 2;
  }
}

/****************************************************************************/
/****************************************************************************/
std::string Disassembler::formatHex(std::stringstream &formatter,
                                    uint16_t value, uint32_t byteLen) {
  formatter.str("");
  formatter << "0x" << std::setfill('0') << std::setw(byteLen * 2) << std::hex
            << value;
  return formatter.str();
}

/****************************************************************************/
/****************************************************************************/
std::string Disassembler::formatRegister(std::stringstream &formatter,
                                         uint32_t regNum,
                                         uint32_t nibbleIndex) {
  // Defines the number of bits to shift a hex value over to the least
  // significant spot
  const uint8_t bitShift = nibbleIndex * 4;
  const uint8_t shiftedRegNum = regNum >> bitShift;
  formatter.str("");
  formatter << "V" << std::setw(1) << std::hex
            << static_cast<int>(shiftedRegNum);
  return formatter.str();
}

/****************************************************************************/
/****************************************************************************/
void Disassembler::formatMemoryAddress(std::stringstream &outputStream,
                                       uint32_t bufferByteIndex) {
  outputStream << "0x" << std::setfill('0') << std::setw(4) << std::hex
               << PROGRAM_START_MEM_ADDRESS + bufferByteIndex << ": ";
}

/****************************************************************************/
/****************************************************************************/
void Disassembler::parseSetRegisterInstr(std::stringstream &formatter,
                                         uint16_t instrRaw,
                                         Instruction &instr) {
  const auto setRegInstrCode =
      static_cast<SetRegInstrCode>(instrRaw & MASK_000F);

  switch (setRegInstrCode) {
  case SetRegInstrCode::LD_VX_VY:
    instr._cmdLabel = "LD";
    instr._msg = "Set Vx = Vy (0x8xy0)";
    break;
  case SetRegInstrCode::OR_VX_VY:
    instr._cmdLabel = "OR";
    instr._msg = "Set Vx = Vx | Vy (0x8xy1)";
    break;
  case SetRegInstrCode::AND_VX_VY:
    instr._cmdLabel = "AND";
    instr._msg = "Set Vx = Vx & Vy (0x8xy2)";
    break;
  case SetRegInstrCode::XOR_VX_VY:
    instr._cmdLabel = "XOR";
    instr._msg = "Set Vx = Vx + Vy ; Set VF = carry (0x8xy4)";
    break;
  case SetRegInstrCode::ADD_VX_VY:
    instr._cmdLabel = "ADD";
    instr._msg = "Set Vx = Vx + Vy ; Set VF = carry (0x8xy4)";
    break;
  case SetRegInstrCode::SUB_VX_VY:
    instr._cmdLabel = "SUB";
    instr._msg = "Set Vx = Vx - Vy ; Set VF = NOT borrow (0x8xy5)";
    break;
  case SetRegInstrCode::SHR_VX_VY:
    instr._cmdLabel = "SHR";
    instr._msg = "Set Vx = Vx / 2 (0x8xy6)";
    break;
  case SetRegInstrCode::SUBN_VX_VY:
    instr._cmdLabel = "SUBN";
    instr._msg = "Set Vx = Vy - Vx, set VF = NOT borrow (0x8xy7)";
    break;
  case SetRegInstrCode::SHL_VX_VY:
    instr._cmdLabel = "SHL";
    instr._msg = "Set Vx = Vx * 2 (0x8xyE)";
    break;
  default:
    // NOTE: this is not a real instruction but used to print raw data to the
    // screen
    instr._cmdLabel = "DATA";
    instr._msg = "Generic data";
    instr._param1 = formatHex(formatter, instrRaw, 2);
    return;
  }

  instr._param1 = formatRegister(formatter, instrRaw & MASK_0F00, 2);
  instr._param2 = formatRegister(formatter, instrRaw & MASK_00F0, 1);
}

/****************************************************************************/
/****************************************************************************/
void Disassembler::parseSkipRegisterInstr(std::stringstream &formatter,
                                          uint16_t instrRaw,
                                          Instruction &instr) {
  const auto skpInstrCode = static_cast<SkpInstrCode>(instrRaw & MASK_00FF);

  switch (skpInstrCode) {
  case SkpInstrCode::SKP_VX:
    instr._cmdLabel = "SKP";
    instr._msg =
        "Skip next instruction if key with the value of Vx is pressed (0xEx9E)";
    break;
  case SkpInstrCode::SKNP_VX:
    instr._cmdLabel = "SKNP";
    instr._msg = "Skip next instruction if the key corresponding to the value "
                 "Vx is in the up position (0xExA1)";
    break;
  default:
    // NOTE: this is not a real instruction but used to print raw data to the
    // screen
    instr._cmdLabel = "DATA";
    instr._msg = "Generic data";
    instr._param1 = formatHex(formatter, instrRaw, 2);
    return;
  }

  instr._param1 = formatRegister(formatter, instrRaw & MASK_0F00, 2);
}

/****************************************************************************/
/****************************************************************************/
void Disassembler::parseIoAndTimerRegisterInstr(std::stringstream &formatter,
                                                uint16_t instrRaw,
                                                Instruction &instr) {
  const auto ioTimerInstrCode =
      static_cast<IoAndTimerCode>(instrRaw & MASK_00FF);

  switch (ioTimerInstrCode) {
  case IoAndTimerCode::LD_VX_DT:
    instr._cmdLabel = "LD";
    instr._msg = "Set Vx=delay timer value (0xFx07)";
    break;
  case IoAndTimerCode::LD_VX_K:
    instr._cmdLabel = "LD";
    instr._msg = "Wait for key press, then store key value in Vx (0xFx0A)";
    break;
  case IoAndTimerCode::LD_DT_VX:
    instr._cmdLabel = "LD";
    instr._msg = "Set delay timer to Vx (0xFx15)";
    break;
  case IoAndTimerCode::LD_ST_VX:
    instr._cmdLabel = "LD";
    instr._msg = "Set sound timer to Vx (0xFx18)";
    break;
  case IoAndTimerCode::ADD_I_VX:
    instr._cmdLabel = "ADD";
    instr._msg = "Set I = I + Vx (0xFx1E)";
    break;
  case IoAndTimerCode::LD_F_VX:
    instr._cmdLabel = "LD";
    instr._msg = "Set I = location of sprite at digit Vx (0xFx29)";
    break;
  case IoAndTimerCode::LD_B_VX:
    instr._cmdLabel = "LD";
    instr._msg = "Store BCD representation of Vx in memory locations I, I+1, "
                 "and I+2 (0xFx33)";
    break;
  case IoAndTimerCode::LD_I_VX:
    instr._cmdLabel = "LD";
    instr._msg =
        "Store register V0-Vx in memory starting at location I (0xFx55)";
    break;
  case IoAndTimerCode::LD_VX_I:
    instr._cmdLabel = "LD";
    instr._msg = "Read registers V0-Vx start at memory location I (0xFx65)";
    break;
  default:
    // NOTE: this is not a real instruction but used to print raw data to the
    // screen
    instr._cmdLabel = "DATA";
    instr._msg = "Generic data";
    instr._param1 = formatHex(formatter, instrRaw, 2);
    return;
  }

  instr._param1 = formatRegister(formatter, instrRaw & MASK_0F00, 2);
}

} // namespace chip8
