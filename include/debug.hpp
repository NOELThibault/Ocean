#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <glad/glad.h>
#include <GL/glext.h>
#include <glm/glm.hpp>
#include <assert.h>

// Checks for all OpenGL errors when called
GLenum glCheckError_( const char * file, int line );
#define glCheckError() glCheckError_( __FILE__, __LINE__ )

// Needs OpenGL 4.X
void APIENTRY glDebugOutput( GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char * message, 
                            const void * userParam );

// Prints matrices
void debugMat( glm::mat4 m );
void debugMat( glm::mat3 m );
void debugMat( glm::mat2 m );


#endif