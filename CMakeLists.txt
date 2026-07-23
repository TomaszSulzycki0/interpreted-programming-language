cmake_minimum_required(VERSION 3.5)
project(ipl)

set(CMAKE_CXX_STANDARD 23)

set(SOURCE_FILES
	src/main.cpp
	src/scope.cpp
	src/parser.cpp
	src/builder.cpp
	src/visitors.cpp
	src/expression.cpp
	src/tokenizer.cpp
	src/abstractNodes.cpp
	src/RPNHandler.cpp
	)

add_executable(ipl ${SOURCE_FILES})

target_compile_options(ipl PRIVATE
    $<$<CONFIG:Debug>:-Wall -Wextra -O0 -g>
)


configure_file(
    "${CMAKE_SOURCE_DIR}/testFiles/input.ipl"
    "${CMAKE_CURRENT_BINARY_DIR}/testFiles/input.ipl"
    COPYONLY
)
