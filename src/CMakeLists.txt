include(antlr4-generator)

include_directories(${ANTLR4_OUTPUTS_DIR})

include_directories(${PROJECT_SOURCE_DIR})

file(GLOB toolman_SOURCE ${PROJECT_SOURCE_DIR}/src/*)

add_executable(toolman ${toolman_SOURCE} ${ANTLR4_CXX_OUTPUTS})

include(antlr4-runtime)
target_link_libraries(toolman antlr4_static)
