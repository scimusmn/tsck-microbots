cmake_minimum_required(VERSION 3.2)

project(path-encoder C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_BINARY_DIR bin)

include_directories(src src/cargs)

add_executable(path-encoder
	src/main.c
	src/image/image.c
	src/options.c)

target_link_libraries(path-encoder m)
