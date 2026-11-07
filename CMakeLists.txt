cmake_minimum_required(VERSION 3.17)
project(linux-pe)

# Header only interface.
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE includes)

# C++20 requirement.
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# Optional C++20 named modules
option(LINUXPE_BUILD_MODULES "Build the linux-pe C++20 modules" OFF)
if(LINUXPE_BUILD_MODULES)
    if(CMAKE_VERSION VERSION_LESS 3.28)
        message(FATAL_ERROR "LINUXPE_BUILD_MODULES requires CMake 3.28 or newer.")
    endif()
    file(GLOB_RECURSE LINUXPE_MODULE_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/modules/*.cppm)
    add_library(${PROJECT_NAME}-modules STATIC)
    add_library(${PROJECT_NAME}::modules ALIAS ${PROJECT_NAME}-modules)
    target_sources(${PROJECT_NAME}-modules PUBLIC FILE_SET CXX_MODULES FILES ${LINUXPE_MODULE_SOURCES})
    target_link_libraries(${PROJECT_NAME}-modules PUBLIC ${PROJECT_NAME})
endif()
