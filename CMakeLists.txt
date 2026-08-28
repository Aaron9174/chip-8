cmake_minimum_required(VERSION 3.31)

project(Chip8Suite LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DISASM_TARGET "chip8_disasm")
set(EMU_TARGET "chip8_emu")
set(COMPILER_OPTIONS "-Wall" "-Wextra" "-Wpedantic")

# Copies roms to the build folder
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/roms/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/roms" FILES_MATCHING PATTERN "*.ch8")

add_subdirectory(src/disassembler)
add_subdirectory(src/emulator)
