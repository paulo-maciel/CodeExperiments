cmake_minimum_required(VERSION 3.1)

#set(CMAKE_C_COMPILER "/usr/bin/g++")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

project(sample)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-std=c++14")
set(CMAKE_CXX_STANDARD_REQUIRED on)

#add_compile_options("-g;-O0")
add_executable(sample sample.cpp)
target_link_libraries( sample pthread )

#include_directories(~/ml2/nova/external/glm/)

#build: cmake -DCMAKE_BUILD_TYPE=Debug ..


