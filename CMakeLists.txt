cmake_minimum_required(VERSION 3.31)

project(Chip8Suite LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set up SDL dependency
list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew" "/usr/local")
find_package(SDL2 CONFIG REQUIRED)

set(DISASM_TARGET "chip8_disasm")
set(EMU_TARGET "chip8_emu")
set(COMPILER_OPTIONS "-Wall" "-Wextra" "-Wpedantic")

add_library(chip8_common INTERFACE)
target_include_directories(chip8_common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/common/include)

# Copies roms to the build folder
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/roms/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/roms" FILES_MATCHING PATTERN "*.ch8")

add_subdirectory(src/disassembler)
add_subdirectory(src/emulator)
