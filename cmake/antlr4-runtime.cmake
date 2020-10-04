cmake_minimum_required(VERSION 3.14)

include(FetchContent)

FetchContent_Declare(
  antlr4-runtime
  URL               https://github.com/antlr/antlr4/archive/4.8.tar.gz
  SOURCE_SUBDIR     runtime/Cpp
)

FetchContent_MakeAvailable(antlr4-runtime)
