add_executable(${DISASM_TARGET} main.cpp Disassembler.cpp)
target_link_libraries(${DISASM_TARGET} PRIVATE chip8_common)
target_sources(${DISASM_TARGET} PRIVATE Disassembler.h)
target_compile_options(${DISASM_TARGET} PRIVATE ${COMPILER_OPTIONS})
