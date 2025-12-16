cmake_minimum_required(VERSION 3.16)
project(project1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Eigen
find_package(Eigen3 REQUIRED NO_MODULE)

include_directories(include)

add_executable(project1
    src/main.cpp
    src/csv_reader.cpp
    src/standardize.cpp
    src/correlation.cpp
    src/csv_writer.cpp
    src/eigen_analysis.cpp
    src/eigen_results_writer.cpp
)

target_link_libraries(project1 Eigen3::Eigen)
