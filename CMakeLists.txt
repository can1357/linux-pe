cmake_minimum_required(VERSION 3.17)
project(linux-pe)

# Header only interface.
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE includes)

# C++20 requirement.
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)