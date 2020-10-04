include(antlr4-generator)

include_directories(${ANTLR4_OUTPUTS_DIR})
add_executable(main main.cc ${ANTLR4_CXX_OUTPUTS})
