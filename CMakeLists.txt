cmake_minimum_required(VERSION 3.0)

project(Ocean)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/glm)
include_directories(${PROJECT_SOURCE_DIR}/include/shader)
include_directories(${ASSIMP_INCLUDE_DIRS})

add_library(glad src/glad.c)
add_library(ldebug src/debug.cpp)
add_library(shader src/shader.cpp)
add_library(camera src/camera.cpp)
add_library(stbi src/stb_image.cpp)
add_library(mesh src/mesh.cpp)
add_library(model src/model.cpp)

target_link_libraries(model PRIVATE mesh)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)

add_executable(Ocean src/main.cpp)

target_link_libraries(Ocean PRIVATE OpenGL::GL glfw assimp glad ldebug shader camera stbi model ${ASSIMP_LIBRARIES})