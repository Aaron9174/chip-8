add_executable(${EMU_TARGET} main.cpp Emulator.cpp)
target_link_libraries(${EMU_TARGET} PRIVATE chip8_common)
target_sources(${EMU_TARGET} PRIVATE Emulator.h)
target_compile_options(${EMU_TARGET} PRIVATE ${COMPILER_OPTIONS})
