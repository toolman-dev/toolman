cmake_minimum_required(VERSION 3.11)

include(FetchContent)

set(SOURCE_SUBDIR "runtime/Cpp")
set(ANTLR4_VERSION "4.8")

FetchContent_Declare(
  antlr4-runtime
  URL               https://github.com/antlr/antlr4/archive/${ANTLR4_VERSION}.tar.gz
  SOURCE_SUBDIR     ${SOURCE_SUBDIR}
)

FetchContent_GetProperties(antlr4-runtime)
if(NOT antlr4-runtime_POPULATED)
  FetchContent_Populate(antlr4-runtime)
  set(__antlr4_src_dir ${antlr4-runtime_SOURCE_DIR}/${SOURCE_SUBDIR})
  if(EXISTS ${__antlr4_src_dir}/CMakeLists.txt)
    add_subdirectory(${__antlr4_src_dir} ${antlr4-runtime_BINARY_DIR})
  endif()
  include_directories(${__antlr4_src_dir}/runtime/src)
  unset(__antlr4_src_dir)
endif()
