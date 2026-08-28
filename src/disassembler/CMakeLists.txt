add_executable(${DISASM_TARGET} main.cpp Disassembler.cpp)
target_sources(${DISASM_TARGET} PRIVATE Disassembler.h)
target_compile_options(${DISASM_TARGET} PRIVATE ${COMPILER_OPTIONS})
