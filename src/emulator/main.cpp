#include "Emulator.h"
#include <fstream>
#include <iostream>
#include <vector>

using namespace chip8;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <path_to_rom.ch8>\n" << std::endl;
    return 1;
  }

  std::string rom_path = argv[1];
  std::ifstream file(rom_path, std::ios::binary | std::ios::ate);

  if (!file.is_open()) {
    std::cerr << "Error: Could not open ROM file: " << rom_path << "\n";
    return 1;
  }

  // Gets the chip8 file size
  std::streamsize bufferSize = file.tellg();

  if (bufferSize == -1) {
    std::cerr << "Error: tellg command failed to calculate file size\n";
    return 1;
  }

  // Return to the front the chip8 binary
  file.seekg(0, std::ios::beg);
  if (file.fail()) {
    std::cerr << "Error: seekg failed to seek to beginning of file\n";
    return 1;
  }

  std::vector<uint8_t> romBuffer(bufferSize);
  if (!file.read(reinterpret_cast<char *>(romBuffer.data()), bufferSize)) {
    std::cerr << "Error: Failed to read ROM data.\n";
    return 1;
  }

  Emulator chip8Emu;
  std::cout << "[Chip8 Emulator] Loaded successfully\n";
  chip8Emu.loadProgram(romBuffer);
  std::cout << "[Chip8 Emulator] Program Loaded successfully\n";

  // Start the emulator
  chip8Emu.start();

  std::stringstream &debugStream = chip8Emu.getDebugStream();
  // Output formatted string stream to file for easy viewing
  std::ofstream outFile("DebugLog.txt");
  if (outFile.is_open()) {
    outFile << debugStream.rdbuf();
    outFile.close();
    std::cout << "[Emulator] Generated Debug Output at "
              << std::filesystem::current_path() << "\n";
  } else {
    std::cerr << "Error: Could not open output file\n";
  }
}
