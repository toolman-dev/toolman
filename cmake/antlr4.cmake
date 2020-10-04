cmake_minimum_required(VERSION 3.7)

include(ExternalProject)

set(ANTLR4_ROOT ${CMAKE_CURRENT_BINARY_DIR}/antlr4_runtime/src/antlr4_runtime)
set(ANTLR4_INCLUDE_DIRS ${ANTLR4_ROOT}/runtime/Cpp/runtime/src)
set(ANTLR4_GIT_REPOSITORY https://github.com/antlr/antlr4.git)
