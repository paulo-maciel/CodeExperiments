cmake_minimum_required(VERSION 3.1)

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang")

project(sample)

set(CMAKE_CXX_STANDARD 14)

#add_compile_options("-g;-O0")
add_executable(sample sample.cpp)

include_directories(~/ml2/nova/external/glm/)

add_library(sampleLib SHARED IMPORTED) # or STATIC instead of SHARED
target_link_libraries(sample LINK_PUBLIC sampleLib)
target_include_directories(sample PUBLIC /usr/local/include)

set_target_properties(sampleLib PROPERTIES
  IMPORTED_LOCATION "/usr/local/lib/libsampleLib.so"
  INTERFACE_INCLUDE_DIRECTORIES "/usr/local/include/"
)

#build: cmake -DCMAKE_BUILD_TYPE=Debug ..
