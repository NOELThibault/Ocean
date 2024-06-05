#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <debug.hpp>
#include <shader.hpp>
#include <camera.hpp>
#include <iostream>

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastMouseX = 0.0f;
float lastMouseY = 0.0f;
Camera cam( glm::vec3( 0.0f, 1.0f, 1.0f ) );

void input( GLFWwindow * window )
{
    CameraMovement direction = NONE;

    if( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
        glfwSetWindowShouldClose( window, true );
    if( glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS )
        direction = FORWARD;
    if( glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS )
        direction = RIGHT;
    if( glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS )
        direction = BACKWARD;
    if( glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS )
        direction = LEFT;

    cam.onKeyboard( direction, deltaTime );
}

void mouseCallBack( GLFWwindow * window , double xPos, double yPos )
{
    float x = static_cast<float>( xPos );
    float y = static_cast<float>( yPos );

    float xOffset = x - lastMouseX;
    float yOffset = lastMouseY - y;

    cam.onMouseMove( xOffset, yOffset );
    lastMouseX = x;
    lastMouseY = y;
}

void scrollCallback( GLFWwindow * window, double xOffset, double yOffset )
{
    cam.onScroll( static_cast<float>( yOffset ) );
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
    const GLFWvidmode * mode = glfwGetVideoMode( glfwGetPrimaryMonitor() );
    glfwWindowHint( GLFW_RED_BITS, mode->redBits );
    glfwWindowHint( GLFW_GREEN_BITS, mode->greenBits );
    glfwWindowHint( GLFW_BLUE_BITS, mode->blueBits );
    glfwWindowHint( GLFW_REFRESH_RATE, mode->refreshRate );

    const int W_WIDTH = mode->width;
    const int W_HEIGHT = mode->height;
    lastMouseX = W_WIDTH / 2.0f;
    lastMouseY = W_HEIGHT / 2.0f;

    GLFWwindow * window = glfwCreateWindow( W_WIDTH, W_HEIGHT, "Ocean", nullptr, nullptr );
    if( !window )
    {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent( window );
    glfwSetCursorPosCallback( window, mouseCallBack );
    glfwSetScrollCallback( window, scrollCallback );
    glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED ); // Cursor captured by window
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );

    // Initialize GLAD    
    if( !gladLoadGLLoader( ( GLADloadproc ) glfwGetProcAddress ) )
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable( GL_DEPTH_TEST );

    // Water surface mesh
    float waterSurface [] = 
    {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f
    };

    unsigned int indices [] = 
    {
        0, 1, 2,
        2, 3, 0
    };

    // Water surface buffers
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays( 1, &VAO );
    glGenBuffers( 1, &VBO );
    glGenBuffers( 1, &EBO );
    glBindVertexArray( VAO );
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( waterSurface ), waterSurface, GL_STATIC_DRAW );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), indices, GL_STATIC_DRAW );

    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), ( void * ) 0 );
    glEnableVertexAttribArray( 0 );

    // Load water surface shader
    Shader shader( "../include/shader/vertex.vs", "../include/shader/fragment.fs" );

    while( !glfwWindowShouldClose( window ) )
    {
        float currentFrame = static_cast<float>( glfwGetTime() );
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        input( window );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glm::mat4 model = glm::mat4( 1.0f );
        glm::mat4 view = cam.getViewMat();
        glm::mat4 projection = glm::mat4( 1.0f );
        projection = glm::perspective( glm::radians( cam.getFov() ), (float)W_WIDTH / (float)W_HEIGHT, 0.1f, 100.0f );
        float angle = -90.0f;
        model = glm::rotate( model, glm::radians( angle ), glm::vec3( 1.0f, 0.0f, 0.0f ) );

        shader.activate();
        shader.setMat4( "model", model );
        shader.setMat4( "view", view );
        shader.setMat4( "projection", projection );

        glBindVertexArray( VAO );
        glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );

        glfwSwapBuffers( window );
        glfwPollEvents();
    }

    glCheckError();
    glDeleteBuffers( 1, &EBO );
    glDeleteBuffers( 1, &VBO );
    glDeleteVertexArrays( 1, &VAO );
    glfwDestroyWindow( window );
    glfwTerminate();
    return 0;
}