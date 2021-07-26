cmake_minimum_required(VERSION 3.9)

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang")

project(sampleLib VERSION 1.0.1 DESCRIPTION "Just a sample lib")

add_library(sampleLib SHARED
    src/sampleLibFile.cpp
)

set_target_properties(sampleLib
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

set_target_properties(sampleLib PROPERTIES VERSION ${PROJECT_VERSION})

set_target_properties(sampleLib PROPERTIES SOVERSION 1)

set_target_properties(sampleLib PROPERTIES PUBLIC_HEADER include/sampleLibFile.h)

target_include_directories(sampleLib PUBLIC include)

include(GNUInstallDirs)

install(TARGETS sampleLib
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})