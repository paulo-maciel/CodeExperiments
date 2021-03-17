cmake_minimum_required(VERSION 3.1)
project(gpuDebug)

add_executable(gpudebug main.cpp DebugGpuAlloc.cpp)

message( ${CMAKE_CURRENT_SOURCE_DIR} )

find_package(OpenGL REQUIRED)
if (OpenGL_FOUND)
  include_directories( ${OPENGL_INCLUDE_DIRS} )
  target_link_libraries(gpudebug ${OPENGL_LIBRARIES})
endif (OpenGL_FOUND)

find_package(GLUT REQUIRED)
if (GLUT_FOUND)
  include_directories( ${GLUT_INCLUDE_DIRS} )
  target_link_libraries(gpudebug ${GLUT_LIBRARY} )
endif (GLUT_FOUND)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_FIND_DEBUG_MODE 1)






include_directories(./)
