#include <debug.hpp>
#include <iostream>
#include <debug.hpp>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

GLenum glCheckError_( const char * file, int line )
{
    GLenum errorCode = glGetError();
    while( ( errorCode != GL_NO_ERROR ) )
    {
        const char * error;
        switch( errorCode )
        {
            case GL_INVALID_ENUM: error = "INVALID ENUM"; break;
            case GL_INVALID_VALUE: error = "INVALID VALUE"; break;
            case GL_INVALID_OPERATION: error = "INVALID OPERATION"; break;
            case GL_OUT_OF_MEMORY: error = "OUT OF MEMEORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID FRAMEBUFFER OPERATION"; break;
            case GL_INVALID_INDEX: error = "INVALID INDEX"; break;
        }
        std::cerr << error << " in " << file << " at line " << line << std::endl;
        errorCode = glGetError();
    }
    return errorCode;
}

void APIENTRY glDebugOutput( GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message, 
                            const void *userParam )
{
    std::cerr << "---------------\n";
    std::cerr << "Debug message (" << id << "): " <<  message << "\n";

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cerr << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cerr << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cerr << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cerr << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cerr << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cerr << "Source: Other"; break;
    } std::cerr << "\n";

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cerr << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cerr << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cerr << "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         std::cerr << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cerr << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cerr << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cerr << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cerr << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cerr << "Type: Other"; break;
    } std::cerr << "\n";
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cerr << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cerr << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cerr << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cerr << "Severity: notification"; break;
    } std::cerr << "\n";
    std::cerr << std::endl;
}

void debugMat( glm::mat4 m )
{
    for( int i = 0; i < 4; i++ )
    {
        for( int j = 0; j < 4; j++ )
            std::cout << m[j][i] << "  ";
        std::cout << "\n";
    }
    std::cout << std::endl;
}

void debugMat( glm::mat3 m )
{
    for( int i = 0; i < 3; i++ )
    {
        for( int j = 0; j < 3; j++ )
            std::cout << m[j][i] << "  ";
        std::cout << "\n";
    }
    std::cout << std::endl;
}

void debugMat( glm::mat2 m )
{
    for( int i = 0; i < 2; i++ )
    {
        for( int j = 0; j < 2; j++ )
            std::cout << m[j][i] << "  ";
        std::cout << "\n";
    }
    std::cout << std::endl;
}