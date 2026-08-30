#include "Disassembler.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
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

  // Deconstruct the binary into assembly
  Disassembler disassembler(romBuffer);
  std::cout << "[Disassembler] Loaded" << std::endl;
  std::stringstream ss;
  disassembler.deconstruct(ss);

  // Output formatted string stream to file for easy viewing
  std::ofstream outFile("romAssembly.txt");
  if (outFile.is_open()) {
    outFile << ss.rdbuf();
    outFile.close();
    std::cout << "[Disassembler] Generated Assembly Output\n";
  } else {
    std::cerr << "Error: Could not open output file\n";
  }
}
