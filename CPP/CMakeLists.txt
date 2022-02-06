cmake_minimum_required(VERSION 3.17.1)
project(ReprojectionLab)

set(CMAKE_CXX_STANDARD 17)

if (WIN32)
     set(SDL2_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../ReprojectionThirdParty/SDL2)
endif()
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../ReprojectionCMakeModules)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Binaries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/Binaries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/Binaries)

add_definitions(-DGLEW_STATIC -DBUILD_DESKTOP)

file(GLOB_RECURSE
     SRC_FILES
     ${PROJECT_SOURCE_DIR}/SourceDesktop/*.h
     ${PROJECT_SOURCE_DIR}/SourceDesktop/*.cpp
     ${PROJECT_SOURCE_DIR}/Content/Shaders/*.vs
     ${PROJECT_SOURCE_DIR}/Content/Shaders/*.ts
     ${PROJECT_SOURCE_DIR}/Content/Shaders/*.gs
     ${PROJECT_SOURCE_DIR}/Content/Shaders/*.fs
     ${PROJECT_SOURCE_DIR}/Content/Shaders/*.cs
     ${PROJECT_SOURCE_DIR}/../ReprojectionThirdParty/Glew/src/glew.c
     ${PROJECT_SOURCE_DIR}/../ReprojectionThirdParty/IMGUI/imgui.cpp
     ${PROJECT_SOURCE_DIR}/../ReprojectionThirdParty/IMGUI/imgui_draw.cpp
     ${PROJECT_SOURCE_DIR}/../ReprojectionThirdParty/IMGUI/imgui_tables.cpp
     ${PROJECT_SOURCE_DIR}/../ReprojectionThirdParty/IMGUI/imgui_widgets.cpp
     ${PROJECT_SOURCE_DIR}/../ReprojectionThirdParty/IMGUI/backends/imgui_impl_sdl.cpp
     ${PROJECT_SOURCE_DIR}/../ReprojectionThirdParty/IMGUI/backends/imgui_impl_opengl3.cpp
     ${PROJECT_SOURCE_DIR}/../ReprojectionLib/Source/*.h
     ${PROJECT_SOURCE_DIR}/../ReprojectionLib/Source/*.cpp
     ${PROJECT_SOURCE_DIR}/../ReprojectionLib/Shaders/*.vs
     ${PROJECT_SOURCE_DIR}/../ReprojectionLib/Shaders/*.ts
     ${PROJECT_SOURCE_DIR}/../ReprojectionLib/Shaders/*.gs
     ${PROJECT_SOURCE_DIR}/../ReprojectionLib/Shaders/*.fs
     ${PROJECT_SOURCE_DIR}/../ReprojectionLib/Shaders/*.cs
     ${PROJECT_SOURCE_DIR}/../ReprojectionLib/Shaders/*.glsl)

add_executable(ReprojectionLab ${SRC_FILES})

target_include_directories(ReprojectionLab
                           ${OPENGL_INCLUDE_DIRS}
                           PRIVATE ${PROJECT_SOURCE_DIR}/../ReprojectionThirdParty/Glew/include
                           PRIVATE ${PROJECT_SOURCE_DIR}/../ReprojectionThirdParty/GLM
                           PRIVATE ${PROJECT_SOURCE_DIR}/../ReprojectionThirdParty/STB
                           PRIVATE ${PROJECT_SOURCE_DIR}/../ReprojectionThirdParty/TinyOBJ
                           PRIVATE ${PROJECT_SOURCE_DIR}/../ReprojectionThirdParty/IMGUI
                           PRIVATE ${PROJECT_SOURCE_DIR}/../ReprojectionThirdParty/IMGUI/backends
                           PRIVATE ${PROJECT_SOURCE_DIR}/../ReprojectionThirdParty/JSON
                           PRIVATE ${PROJECT_SOURCE_DIR}/../ReprojectionThirdParty/PortableFileDialogs
                           PRIVATE ${PROJECT_SOURCE_DIR}/../ReprojectionLib/Source)

target_link_libraries(ReprojectionLab
                      ${OPENGL_LIBRARIES}
                      ${CMAKE_DL_LIBS}
                      SDL2::Main)

source_group("CMake" CMakeLists.txt)
source_group("Glew" Glew/*.*)
source_group("IMGUI" IMGUI/*.*)
source_group("Shaders" Shaders/*.*)
source_group("ReprojectionLib", ReprojectionLib/*.*)