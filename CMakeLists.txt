project(toolman)

# minimum required CMAKE version
CMAKE_MINIMUM_REQUIRED(VERSION 3.14 FATAL_ERROR)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

add_subdirectory(src)