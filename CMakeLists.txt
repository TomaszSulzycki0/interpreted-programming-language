cmake_minimum_required(VERSION 3.20)
project(ipl)

add_library(ipl_lib STATIC)

target_compile_features(ipl_lib PUBLIC cxx_std_23)

target_sources(ipl_lib PRIVATE
    src/scope.cpp
    src/parser.cpp
    src/builder.cpp
    src/visitors.cpp
    src/expression.cpp
    src/tokenizer.cpp
    src/abstractNodes.cpp
    src/RPNHandler.cpp
    src/typeChecker.cpp
)

target_include_directories(ipl_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(MSVC)
    target_compile_options(ipl_lib PRIVATE /W4 /WX)
else()
    target_compile_options(ipl_lib PRIVATE 
        -Wall 
        -Wextra 
        -Wpedantic
        -Werror
        $<$<CONFIG:Debug>:-O0 -g>
        $<$<CONFIG:Release>:-O3>
    )
endif()

target_compile_definitions(ipl_lib PRIVATE
    $<$<CONFIG:Debug>:DEBUG_ALL>
)

add_executable(ipl src/main.cpp)
target_link_libraries(ipl PRIVATE ipl_lib)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/testFiles/input.ipl"
    "${CMAKE_CURRENT_BINARY_DIR}/testFiles/input.ipl"
    COPYONLY
)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.15.2
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

add_executable(unit_tests)

target_sources(unit_tests PRIVATE
    tests/parser_declaration_test.cpp
    tests/parser_expression_test.cpp
    tests/parser_error_test.cpp
    tests/parser_control_flow_test.cpp
    tests/tokenizer_test.cpp
    tests/typechecker_declaration_test.cpp
    tests/typechecker_assignment_test.cpp
)

target_link_libraries(unit_tests PRIVATE 
    ipl_lib 
    GTest::gtest_main
)

gtest_discover_tests(unit_tests)
