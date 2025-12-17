cmake_minimum_required(VERSION 3.14)
project(three_des_app CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)

add_executable(three_des
    src/main.cpp
    src/crypto3des.cpp
)

target_include_directories(three_des PRIVATE include)
target_link_libraries(three_des PRIVATE OpenSSL::Crypto)
