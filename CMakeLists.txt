cmake_minimum_required(VERSION 3.17)
project(OpenGLProject)

set(CMAKE_CXX_STANDARD 14)
link_directories("C:/Program Files/mingw-w64/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64/lib")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -D FREEGLUT_STATIC")
add_executable(OpenGLProject  main.cpp)

target_link_libraries(OpenGLProject freeglut_static opengl32 winmm gdi32 glu32.lib glaux.lib)