#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <debug.hpp>
#include <shader.hpp>
#include <camera.hpp>
#include <iostream>
#include <vector>
#include <stb_image.h>

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
    unsigned int waterVAO, waterVBO, waterEBO;
    glGenVertexArrays( 1, &waterVAO );
    glGenBuffers( 1, &waterVBO );
    glGenBuffers( 1, &waterEBO );
    glBindVertexArray( waterVAO );
    glBindBuffer( GL_ARRAY_BUFFER, waterVBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( waterSurface ), waterSurface, GL_STATIC_DRAW );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, waterEBO );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), indices, GL_STATIC_DRAW );

    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), ( void * ) 0 );
    glEnableVertexAttribArray( 0 );

    // Load water surface shader
    Shader waterShader( "../include/shader/water.vs", "../include/shader/water.fs" );

    // Skybox mesh
    float skyboxVertices [] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // Skybox buffers
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays( 1, &skyboxVAO );
    glGenBuffers( 1, &skyboxVBO );
    glBindVertexArray( skyboxVAO );
    glBindBuffer( GL_ARRAY_BUFFER, skyboxVBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( skyboxVertices ), &skyboxVertices, GL_STATIC_DRAW );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), ( void* )0 );

    // Load skybox texture
    std::vector<std::string> faces
    {
        "../include/skybox/right.jpg",
        "../include/skybox/left.jpg",
        "../include/skybox/top.jpg",
        "../include/skybox/bottom.jpg",
        "../include/skybox/front.jpg",
        "../include/skybox/back.jpg",
    };
    unsigned int skyTextID;
    glGenTextures( 1, &skyTextID );
    glBindTexture( GL_TEXTURE_CUBE_MAP, skyTextID );
    int width, height, nrChannels;
    for ( int i = 0; i < faces.size(); i++ )
    {
        unsigned char * data = stbi_load( faces[i].c_str(), &width, &height, &nrChannels, 0 );
        if( data )
        {
            glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
            stbi_image_free( data );
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free( data );
        }
    }
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

    // Load skybox shader
    Shader skyboxShader( "../include/shader/skybox.vs", "../include/shader/skybox.fs" );
    skyboxShader.activate();
    skyboxShader.setInt( "skybox", 0 );

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

        waterShader.activate();
        waterShader.setMat4( "model", model );
        waterShader.setMat4( "view", view );
        waterShader.setMat4( "projection", projection );

        glBindVertexArray( waterVAO );
        glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );

        glDepthFunc( GL_LEQUAL );
        view = glm::mat4( glm::mat3( cam.getViewMat() ) ); // remove translation from the view matrix
        skyboxShader.activate();
        skyboxShader.setMat4( "view", view );
        skyboxShader.setMat4( "projection", projection );

        glBindVertexArray( skyboxVAO );
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_CUBE_MAP, skyTextID );
        glDrawArrays( GL_TRIANGLES, 0, 36 );
        glDepthFunc( GL_LESS );

        glfwSwapBuffers( window );
        glfwPollEvents();
    }

    glCheckError();
    glDeleteBuffers( 1, &skyboxVBO );
    glDeleteVertexArrays( 1, &skyboxVAO );
    glDeleteBuffers( 1, &waterEBO );
    glDeleteBuffers( 1, &waterVBO );
    glDeleteVertexArrays( 1, &waterVAO );
    glfwDestroyWindow( window );
    glfwTerminate();
    return 0;
}