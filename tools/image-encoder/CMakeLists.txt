cmake_minimum_required(VERSION 3.2)

project(image-encoder C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_BINARY_DIR bin)

include_directories(src src/cargs)

add_executable(image-encoder
	src/main.c
	src/cargs/cargs.c
	src/image/image.c
	src/options.c)

target_link_libraries(image-encoder m)


add_executable(test-image-encoder
	EXCLUDE_FROM_ALL
	src/test/lily-test.c
	src/image/image.c
	src/image/image.test.c
	src/processing/proc.c
	src/processing/proc.test.c
	src/test/tests.c)

target_compile_definitions(test-image-encoder PUBLIC SOURCE_PATH_SIZE=65)
target_link_libraries(test-image-encoder m)
