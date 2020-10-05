project(toolman)

# minimum required CMAKE version
cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

add_subdirectory(src)
