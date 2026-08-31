add_executable(${EMU_TARGET} main.cpp Emulator.cpp Display.cpp)
target_link_libraries(${EMU_TARGET} PRIVATE chip8_common SDL2::SDL2)
target_sources(${EMU_TARGET} PRIVATE Emulator.h Display.h)
target_compile_options(${EMU_TARGET} PRIVATE ${COMPILER_OPTIONS})
