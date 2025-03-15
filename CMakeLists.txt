cmake_minimum_required(VERSION 3.15)
project(Ocean LANGUAGES C CXX)

# Set C++ standard and policies
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Find dependencies using modern CMake
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(assimp REQUIRED)
find_package(Freetype REQUIRED)

# Create library targets first
add_library(glad src/glad.c)
add_library(ldebug src/debug.cpp)
add_library(shader src/shader.cpp)
add_library(camera src/camera.cpp)
add_library(stbi src/stb_image.cpp)
add_library(mesh src/mesh.cpp)
add_library(model src/model.cpp)

# Main executable
add_executable(Ocean src/main.cpp)

# Set common include directories for all targets
foreach(target IN ITEMS glad ldebug shader camera stbi mesh model Ocean)
    target_include_directories(${target} PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/glm
        ${PROJECT_SOURCE_DIR}/include/shader
    )    
endforeach()    

# Link dependencies for specific targets
target_link_libraries(model PRIVATE mesh assimp::assimp)

# Special handling for glad (C library)
target_include_directories(glad PRIVATE ${OPENGL_INCLUDE_DIR})

# Link all components to main executable
target_link_libraries(Ocean PRIVATE
    OpenGL::GL
    glfw
    assimp::assimp
    glad
    ldebug
    shader
    camera
    stbi
    model
    Freetype::Freetype
)