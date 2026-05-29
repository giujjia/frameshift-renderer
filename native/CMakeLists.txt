cmake_minimum_required(VERSION 3.10)
project(ShiftGL VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(include)

add_executable(ShiftGL_Test src/renderer.cpp src/glad.c)

target_link_libraries(ShiftGL_Test glfw OpenGL::GL)
