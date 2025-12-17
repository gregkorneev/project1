cmake_minimum_required(VERSION 3.14)
project(DZ1_Variant14 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Все бинарники будут лежать в build/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(ML-part1)
add_subdirectory(3DES-part2)

add_executable(launcher src/launcher.cpp)
