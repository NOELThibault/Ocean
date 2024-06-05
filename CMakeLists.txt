cmake_minimum_required(VERSION 3.0)

project(Ocean)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/glm)
include_directories(${PROJECT_SOURCE_DIR}/include/shader)

add_library(glad src/glad.c)
add_library(ldebug src/debug.cpp)
add_library(shader src/shader.cpp)
add_library(camera src/camera.cpp)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(Ocean src/main.cpp)

target_link_libraries(Ocean PRIVATE OpenGL::GL glfw glad ldebug shader camera)