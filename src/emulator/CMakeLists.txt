add_executable(${EMU_TARGET} main.cpp cpu.cpp)
target_sources(${EMU_TARGET} PRIVATE cpu.h)
target_compile_options(${EMU_TARGET} PRIVATE ${COMPILER_OPTIONS})
