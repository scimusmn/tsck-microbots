cmake_minimum_required(VERSION 3.2)

project(image-encoder C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_BINARY_DIR bin)

include_directories(src)

add_executable(image-encoder
	src/main.c
	src/cargs.c
	src/options.c)

target_link_libraries(image-encoder m)


add_executable(test-image-encoder
	EXCLUDE_FROM_ALL
	src/lily-test.c
	src/image.c
	src/image.test.c
	src/tests.c)

target_compile_definitions(test-image-encoder PUBLIC SOURCE_PATH_SIZE=65)
target_link_libraries(test-image-encoder m)
