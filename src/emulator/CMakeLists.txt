add_executable(${EMU_TARGET} main.cpp Emulator.cpp)
target_sources(${EMU_TARGET} PRIVATE Emulator.h)
target_compile_options(${EMU_TARGET} PRIVATE ${COMPILER_OPTIONS})
