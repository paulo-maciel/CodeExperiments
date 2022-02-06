cmake_minimum_required(VERSION 3.17.1)
project(CodeExperiments)

set(CMAKE_CXX_STANDARD 17)

if (WIN32)
     set(SDL2_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../ExternalLibs/SDL2)
endif()
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../CodeExperimentsCMakeModules)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Binaries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/Binaries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/Binaries)

add_definitions(-DGLEW_STATIC -DBUILD_DESKTOP)

file(GLOB_RECURSE
     SRC_FILES
     ${PROJECT_SOURCE_DIR}/include/*.h
     ${PROJECT_SOURCE_DIR}/src/*.cpp
     ${PROJECT_SOURCE_DIR}/Content/Shaders/*.vs
     ${PROJECT_SOURCE_DIR}/Content/Shaders/*.ts
     ${PROJECT_SOURCE_DIR}/Content/Shaders/*.gs
     ${PROJECT_SOURCE_DIR}/Content/Shaders/*.fs
     ${PROJECT_SOURCE_DIR}/Content/Shaders/*.cs
     ${PROJECT_SOURCE_DIR}/lib/src/sampleLibFile.cpp
     ${PROJECT_SOURCE_DIR}/../ExternalLibs/Glew/src/glew.c
     ${PROJECT_SOURCE_DIR}/../ExternalLibs/IMGUI/imgui.cpp
     ${PROJECT_SOURCE_DIR}/../ExternalLibs/IMGUI/imgui_draw.cpp
     ${PROJECT_SOURCE_DIR}/../ExternalLibs/IMGUI/imgui_tables.cpp
     ${PROJECT_SOURCE_DIR}/../ExternalLibs/IMGUI/imgui_widgets.cpp
     ${PROJECT_SOURCE_DIR}/../ExternalLibs/IMGUI/backends/imgui_impl_sdl.cpp
     ${PROJECT_SOURCE_DIR}/../ExternalLibs/IMGUI/backends/imgui_impl_opengl3.cpp
     ${PROJECT_SOURCE_DIR}/../CodeExperimentsLib/Source/*.h
     ${PROJECT_SOURCE_DIR}/../CodeExperimentsLib/Source/*.cpp
     ${PROJECT_SOURCE_DIR}/../CodeExperimentsLib/Shaders/*.vs
     ${PROJECT_SOURCE_DIR}/../CodeExperimentsLib/Shaders/*.ts
     ${PROJECT_SOURCE_DIR}/../CodeExperimentsLib/Shaders/*.gs
     ${PROJECT_SOURCE_DIR}/../CodeExperimentsLib/Shaders/*.fs
     ${PROJECT_SOURCE_DIR}/../CodeExperimentsLib/Shaders/*.cs
     ${PROJECT_SOURCE_DIR}/../CodeExperimentsLib/Shaders/*.glsl)

add_executable(CodeExperiments ${SRC_FILES})

target_include_directories(CodeExperiments
                           ${OPENGL_INCLUDE_DIRS}
                           PRIVATE ${PROJECT_SOURCE_DIR}/../ExternalLibs/Glew/include
                           PRIVATE ${PROJECT_SOURCE_DIR}/../ExternalLibs/GLM
                           PRIVATE ${PROJECT_SOURCE_DIR}/../ExternalLibs/STB
                           PRIVATE ${PROJECT_SOURCE_DIR}/../ExternalLibs/TinyOBJ
                           PRIVATE ${PROJECT_SOURCE_DIR}/../ExternalLibs/IMGUI
                           PRIVATE ${PROJECT_SOURCE_DIR}/../ExternalLibs/IMGUI/backends
                           PRIVATE ${PROJECT_SOURCE_DIR}/../ExternalLibs/JSON
                           PRIVATE ${PROJECT_SOURCE_DIR}/../ExternalLibs/PortableFileDialogs
                           PRIVATE ${PROJECT_SOURCE_DIR}/../CodeExperimentsLib/Source)

target_link_libraries(CodeExperiments
                      ${OPENGL_LIBRARIES}
                      ${CMAKE_DL_LIBS}
                      SDL2::Main)

source_group("CMake" CMakeLists.txt)
source_group("Glew" Glew/*.*)
source_group("IMGUI" IMGUI/*.*)
source_group("Shaders" Shaders/*.*)
source_group("CodeExperimentsLib", CodeExperimentsLib/*.*)