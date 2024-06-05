#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <debug.hpp>
#include <iostream>

void input( GLFWwindow * window )
{
    if( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
        glfwSetWindowShouldClose( window, true );
}

int main()
{
    // Initialize GLFW
    if( !glfwInit() )
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    // Set OpenGL version to 3.3
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    // Create window
    GLFWwindow * window = glfwCreateWindow( 800, 600, "Ocean", glfwGetPrimaryMonitor(), nullptr );
    if( !window )
    {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent( window );
    
    // Initialize GLAD    
    if( !gladLoadGLLoader( ( GLADloadproc ) glfwGetProcAddress ) )
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }  

    while( !glfwWindowShouldClose( window ) )
    {
        glClear( GL_COLOR_BUFFER_BIT );
        input( window );
        glfwSwapBuffers( window );
        glfwPollEvents();
    }

    glCheckError();
    glfwDestroyWindow( window );
    glfwTerminate();
    return 0;
}