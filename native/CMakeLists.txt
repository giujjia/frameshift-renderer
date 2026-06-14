cmake_minimum_required(VERSION 3.10)
project(ShiftGL VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(JNI REQUIRED)
find_package(Freetype REQUIRED)

include_directories(
    include 
    ${JNI_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
)

# Biblioteca compartilhada JNI carregada pelo Java
add_library(motor SHARED 
    src/bridge.cpp 
    src/renderer.cpp
    src/text_renderer.cpp
    src/glad.c
)
target_link_libraries(motor 
    glfw 
    OpenGL::GL
    ${FREETYPE_LIBRARIES}
)

add_executable(ShiftGL_Test 
    src/renderer.cpp 
    src/text_renderer.cpp
    src/glad.c
)
target_link_libraries(ShiftGL_Test 
    glfw 
    OpenGL::GL
    ${FREETYPE_LIBRARIES}
)
target_compile_definitions(ShiftGL_Test PRIVATE SHIFTGL_STANDALONE)


