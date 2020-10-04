# - Generate lexer and paraser source code
#
# -------------------------
# Once done this will define
#
#  ANTLR4_OUTPUTS_DIR - The antlr4 outputs dir.
#  ANTLR4_CXX_OUTPUTS - The antlr4 cpp output files list.
#  ANTLR4_OUTPUTS - The antlr4 other output files list.
#
# e.g.
# # include generated files in project environment
# include_directories(${ANTLR4_OUTPUTS_DIR})
#
# add generated grammar to demo binary target
# add_executable(main main.cpp ${ANTLR4_CXX_OUTPUTS})
#

set (ANTLR4_OUTPUTS_DIR "${PROJECT_BINARY_DIR}/mvn_target/generated-sources/antlr4")

if(NOT EXISTS ${ANTLR4_OUTPUTS_DIR})
    find_package(Java QUIET COMPONENTS Runtime)

    find_package(Maven REQUIRED)

    execute_process(
        COMMAND ${Maven_EXECUTABLE} -f "${PROJECT_SOURCE_DIR}/grammer/pom.xml" antlr4:antlr4 "-Dantlr.build.target.dir=${PROJECT_BINARY_DIR}/mvn_target"
        OUTPUT_VARIABLE ANTLR_MVN_OUTPUT
        ERROR_VARIABLE ANTLR_MVN_ERROR
        RESULT_VARIABLE ANTLR_MVN_RESULT
    )

    if(NOT ANTLR_MVN_RESULT EQUAL 0)
        message(
            FATAL_ERROR
            "antlr4 generate code failed with the output '${ANTLR_MVN_ERROR}'")
    endif()

endif()

file(GLOB ANTLR4_CXX_OUTPUTS "${ANTLR4_OUTPUTS_DIR}/*.h" "${ANTLR4_OUTPUTS_DIR}/*.cpp")
file(GLOB ANTLR4_OUTPUTS "${ANTLR4_OUTPUTS_DIR}/*.tokens" "${ANTLR4_OUTPUTS_DIR}/*.interp")
