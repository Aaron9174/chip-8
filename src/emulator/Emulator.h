#include <array>
#include <stack>
#include <vector>

constexpr uint32_t TOTAL_MEMORY_SIZE_BYTES = 4096;
constexpr uint8_t TOTAL_GPR_SIZE = 16;
constexpr uint8_t TOTAL_STACK_SIZE = 16;
constexpr double TARGET_HZ = 60;
// TODO: move to common
constexpr uint8_t PROGRAM_START_MEM_ADDRESS = 0x200;

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
  void initialize();

  // Memory structure for RAM
  std::array<uint8_t, TOTAL_MEMORY_SIZE_BYTES> _memory;

  // Stack
  std::array<uint16_t, TOTAL_STACK_SIZE> _stack;

  // Register used to store memory addresses
  // NOTE: only the three LSB nibbles are used
  RegisterSize16 _instrReg;

  // General purpose registers (Vx registers)
  std::array<RegisterSize8, TOTAL_GPR_SIZE> _generalPurposeReg;

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
