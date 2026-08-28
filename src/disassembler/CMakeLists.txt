add_executable(${DISASM_TARGET} main.cpp disassembler.cpp)
target_sources(${DISASM_TARGET} PRIVATE disassembler.h)
target_compile_options(${DISASM_TARGET} PRIVATE ${COMPILER_OPTIONS})
