cmake_minimum_required(VERSION 3.5)
project(ipl)

set(CMAKE_CXX_STANDARD 23)

set(SOURCE_FILES
	src/main.cpp
	)

add_executable(ipl ${SOURCE_FILES})

target_compile_options(ipl PRIVATE
    $<$<CONFIG:Debug>:-Wall -Wextra -O0 -g>
)


add_custom_command(TARGET ipl POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
    		${CMAKE_SOURCE_DIR}/testFiles/input.ipl
    		$<TARGET_FILE_DIR:ipl>/testFiles/input.ipl
)
