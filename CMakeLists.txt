cmake_minimum_required(VERSION 3.2)

project(microbots CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

add_executable(microbots
  src/Button.cpp
  src/Stepper.cpp
  src/Stage.cpp
  src/main.cpp)

target_link_libraries(microbots wiringPi sfml-graphics sfml-audio)
