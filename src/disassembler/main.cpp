#include <fstream>
#include <iostream>
#include <string>
#include <vector>

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
  std::streamsize size = file.tellg();

  if (size == -1) {
    std::cerr << "Error: tellg command failed to calculate file size\n";
    return 1;
  }

  // Return to the front the chip8 binary
  file.seekg(0, std::ios::beg);
  if (file.fail()) {
    std::cerr << "Error: seekg failed to seek to beginning of file\n";
    return 1;
  }

  std::vector<uint8_t> rom_buffer(size);
  if (!file.read(reinterpret_cast<char *>(rom_buffer.data()), size)) {
    std::cerr << "Error: Failed to read ROM data.\n";
    return 1;
  }

  std::cout << "[Disassembler] Loaded" << std::endl;

  // TODO: put disassembler logic here
}
