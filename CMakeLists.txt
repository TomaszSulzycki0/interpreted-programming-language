cmake_minimum_required(VERSION 3.20)
project(ipl)

set(CMAKE_CXX_STANDARD 23)

add_executable(ipl)

target_compile_features(ipl PRIVATE cxx_std_23)

target_sources(ipl PRIVATE
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

target_include_directories(ipl PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(MSVC)
    target_compile_options(ipl PRIVATE /W4 /WX)
else()
    target_compile_options(ipl PRIVATE 
        -Wall 
        -Wextra 
        -Wpedantic
		-Werror
        $<$<CONFIG:Debug>:-O0 -g>
        $<$<CONFIG:Release>:-O3>
    )
endif()


configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/testFiles/input.ipl"
    "${CMAKE_CURRENT_BINARY_DIR}/testFiles/input.ipl"
    COPYONLY
)
