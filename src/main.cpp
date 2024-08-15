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
#include <model.hpp>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H

#define FAR_PLANE 100.0f
#define NEAR_PLANE 0.1f

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastMouseX = 0.0f;
float lastMouseY = 0.0f;
Camera cam( glm::vec3( 0.0f, 5.0f, 0.0f ) );

unsigned int numWaves = 16;
float fogStart = 20.0f;
float fogEnd = 100.0f;
float fogHeight = 0.05f;
float gammaCorrection = 2.2f;
float amplitude = 1.0f;
float frequency = 1.0f;
float speed = 1.0f;
float amplDecay = 0.82f;
float waveLenIncrease = 1.18f;
float k = 0.5;
float ambient = 0.1f;
float shininess = 64.0f;
float fresnel = 0.4f;

struct Character
{
    int characterId;
    glm::ivec2 size;
    glm::ivec2 bearing;
    unsigned int xAdvance;
};
float textVertices [] = {
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 0.0f
};

std::map< char, Character > characters;
unsigned int textArray;
glm::mat4 textTransforms[ 200 ];
int letterMap[ 200 ];
unsigned int textVAO, textVBO;

int loadFont( FT_Library ft, FT_Face face )
{
    FT_Set_Pixel_Sizes( face, 256, 256 );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    glGenTextures( 1, &textArray );
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D_ARRAY, textArray );
    glCheckError();
    glTexImage3D( GL_TEXTURE_2D_ARRAY, 0, GL_R8, 256, 256, 128, 0, GL_RED, GL_UNSIGNED_BYTE, 0 );

    for( unsigned char c = 0; c < 128; c++ )
    {
        if( FT_Load_Char( face, c, FT_LOAD_RENDER ) )
        {
            std::cerr << "Failed to load Glyph" << std::endl;
            return -1;
        }
        // Load the texture
        glTexSubImage3D( GL_TEXTURE_2D_ARRAY, 0, 0, 0, int( c ), face->glyph->bitmap.width, face->glyph->bitmap.rows, 1, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer );
        glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        Character character = {
            int( c ),
            glm::ivec2( face->glyph->bitmap.width, face->glyph->bitmap.rows ),
            glm::ivec2( face->glyph->bitmap_left, face->glyph->bitmap_top ),
            static_cast<unsigned int>( face->glyph->advance.x )
        };
        characters.insert( std::pair< char, Character >( c, character ) );
    }
    glCheckError();

    glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

    FT_Done_Face( face );
    FT_Done_FreeType( ft );

    for( int i = 0; i < 200; i++ )
    {
        textTransforms[i] = glm::mat4( 1.0f );
        letterMap[i] = 0;
    }
    // Set up buffers
    glCheckError();
    glGenVertexArrays( 1, &textVAO );
    glGenBuffers( 1, &textVBO );
    glBindVertexArray( textVAO );
    glBindBuffer( GL_ARRAY_BUFFER, textVBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( textVertices ), textVertices, GL_STATIC_DRAW );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray( 0 );
    return 0;
}

void renderText( const std::string & text, Shader & shader, float x, float y, float scale, glm::vec3 & color )
{
    glDisable( GL_DEPTH_TEST );
    float ogX = x;
    shader.activate();
    shader.setVec3( "textColor", color );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D_ARRAY, textArray );
    glBindVertexArray( textVAO );
    glBindBuffer( GL_ARRAY_BUFFER, textVBO );

    int currentIndex = 0;
    std::string::const_iterator c;
    for( c = text.begin(); c != text.end(); c++ )
    {
        Character ch = characters[ *c ];
        switch( *c )
        {
            case '\n':
                y -= ch.size.y * 1.5f * scale;
                x = ogX;
                break;
            case ' ':
                x += ( ch.xAdvance >> 6 ) * scale;
                break;
            case '\t':
                x += 4 * ( ch.xAdvance >> 6 ) * scale;
                break;
            default:
                float xpos = x + ch.bearing.x * scale;
                float ypos = y - ( 256 - ch.bearing.y ) * scale;
                float w = ch.size.x * scale;
                float h = ch.size.y * scale;

                textTransforms[ currentIndex ] = glm::translate( glm::mat4( 1.0f ), glm::vec3( xpos, ypos, 0.0f ) ) * glm::scale( glm::mat4( 1.0f ), glm::vec3( 256.0f * scale, 256.0f * scale, 0.0f ) );
                letterMap[ currentIndex ] = ch.characterId;

                x += ( ch.xAdvance >> 6 ) * scale;
                
                currentIndex++;
                break;
        }
    }
    shader.setMat4Array( "transforms", textTransforms, currentIndex );
    shader.setIntArray( "letterMap", letterMap, currentIndex );
    glDrawArraysInstanced( GL_TRIANGLE_STRIP, 0, 4, currentIndex );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray( 0 );
    glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );
    glEnable( GL_DEPTH_TEST );
}

void move( GLFWwindow * window )
{
    CameraMovement direction = NONE;

    if( glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS )
        direction = FORWARD;
    if( glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS )
        direction = RIGHT;
    if( glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS )
        direction = BACKWARD;
    if( glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS )
        direction = LEFT;
    if( glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS && glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS )
        direction = FORWARD_RIGHT;
    if( glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS && glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS )
        direction = BACKWARD_RIGHT;
    if( glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS && glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS )
        direction = BACKWARD_LEFT;
    if( glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS && glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS )
        direction = FORWARD_LEFT;
    if( glfwGetKey( window, GLFW_KEY_SPACE ) == GLFW_PRESS )
        direction = UP;
    if( glfwGetKey( window, GLFW_KEY_LEFT_SHIFT ) == GLFW_PRESS )
        direction = DOWN;

    cam.onKeyboard( direction, deltaTime );
}

void keyCallBack( GLFWwindow * window, int key, int scancode, int action, int mods )
{
    switch( key )
    {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose( window, true );
            break;
        case GLFW_KEY_1:
            if( numWaves > 0 )
                numWaves--;
            break;
        case GLFW_KEY_2:
            if( numWaves < 64 )
                numWaves++;
            break;
        case GLFW_KEY_0:
            if( gammaCorrection < 5.0f )
                gammaCorrection += 0.1f;
            break;
        case GLFW_KEY_9:
            if( gammaCorrection > 0.2f )
                gammaCorrection -= 0.1f;
            break;
        case GLFW_KEY_Z:
            if( fogStart > NEAR_PLANE + 0.5f )
                fogStart -= 0.5f;
            break;
        case GLFW_KEY_X:
            if( fogStart < fogEnd )
                fogStart += 0.5f;
            break;
        case GLFW_KEY_E:
            if( fogEnd < FAR_PLANE - 0.5f )
                fogEnd += 0.5f;
            break;
        case GLFW_KEY_Q:
            if( fogEnd > fogStart )
                fogEnd -= 0.5f;
            break;
        case GLFW_KEY_C:
            if( fogHeight > 0.0f )
                fogHeight -= 0.01f;
            break;
        case GLFW_KEY_V:
            if( fogHeight < 1.0f )
                fogHeight += 0.01f;
            break;
        case GLFW_KEY_UP:
            if( amplitude < 10.0f )
                amplitude += 0.1f;
            break;
        case GLFW_KEY_DOWN:
            if( amplitude > 0.1f )
                amplitude -= 0.1f;
            break;
        case GLFW_KEY_LEFT:
            if( frequency > 0.1f )
                frequency -= 0.1f;
            break;
        case GLFW_KEY_RIGHT:
            if( frequency < 10.0f )
                frequency += 0.1f;
            break;
        case GLFW_KEY_KP_8:
            if( amplDecay < 0.98f )
                amplDecay += 0.01f;
            break;
        case GLFW_KEY_KP_5:
            if( amplDecay > 0.02f )
                amplDecay -= 0.01f;
            break;
        case GLFW_KEY_KP_4:
            if( waveLenIncrease > 1.02f )
                waveLenIncrease -= 0.01f;
            break;
        case GLFW_KEY_KP_6:
            if( waveLenIncrease < 2.0f )
                waveLenIncrease += 0.01f;
            break;
        case GLFW_KEY_KP_7:
            if( k > 0.0f )
                k -= 0.01f;
            break;
        case GLFW_KEY_KP_9:
            if( k < 1.0f )
                k += 0.01f;
            break;
        case GLFW_KEY_KP_1:
            if( speed > 0.1f )
                speed -= 0.1f;
            break;
        case GLFW_KEY_KP_3:
            if( speed < 7.0f )
                speed += 0.1f;
            break;
        case GLFW_KEY_J:
            if( ambient > 0.0f )
                ambient -= 0.01f;
            break;
        case GLFW_KEY_K:
            if( ambient < 1.0f )
                ambient += 0.01f;
            break;
        case GLFW_KEY_L:
            if( shininess < 256.0f )
                shininess += 1.0f;
            break;
        case GLFW_KEY_H:
            if( shininess > 1.0f )
                shininess -= 1.0f;
            break;
        case GLFW_KEY_F:
            if( fresnel > 0.0f )
                fresnel -= 0.01f;
            break;
        case GLFW_KEY_G:
            if( fresnel < 1.0f )
                fresnel += 0.01f;
            break;
    }
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
    glfwSetKeyCallback( window, keyCallBack );
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
    glEnable( GL_CULL_FACE );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    // Load text shader
    Shader textShader( "../include/shader/text.vs", "../include/shader/text.fs" );
    glm::mat4 screenProjection = glm::ortho( 0.0f, static_cast< float >( W_WIDTH ), 0.0f, static_cast< float >( W_HEIGHT ) );
    textShader.activate();
    textShader.setMat4( "projection", screenProjection );

    // Initialize text rendering
    FT_Library ft;
    if( FT_Init_FreeType( &ft ) )
    {
        std::cerr << "Failed to initialize FreeType" << std::endl;
        return -1;
    }
    FT_Face face;
    if( FT_New_Face( ft, "../include/font/arial.ttf", 0, &face ) )
    {
        std::cerr << "Failed to load font" << std::endl;
        return -1;
    }
    if( loadFont( ft, face ) == -1 )
    {
        std::cerr << "Failed to load font" << std::endl;
        return -1;
    }
    glCheckError();

    // Water surface model
    Model water( "../include/water.obj" );
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

    glm::vec3 fogColor = glm::vec3( 0.5f, 0.5f, 0.5f );

    glm::vec3 textColor = glm::vec3( 1.0f, 1.0f, 0.0f );

    unsigned long frameCount = 0;
    float sumFPS = 0.0f;
    float avgFPS = 0.0f;
    unsigned long countFPS = 1;
    while( !glfwWindowShouldClose( window ) )
    {
        float currentFrame = static_cast<float>( glfwGetTime() );
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        frameCount++;
        if( frameCount % 60 == 59 )
        {
            avgFPS += sumFPS / 60.0f;
            countFPS++;
            sumFPS = 0.0f;
        }
        sumFPS += 1.0f / deltaTime;

        move( window );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glm::mat4 view = cam.getViewMat();
        glm::mat4 projection = glm::mat4( 1.0f );
        projection = glm::perspective( glm::radians( cam.getFov() ), (float)W_WIDTH / (float)W_HEIGHT, NEAR_PLANE, FAR_PLANE );

        waterShader.activate();
        waterShader.setMat4( "view", view );
        waterShader.setMat4( "projection", projection );
        waterShader.setInt( "numWaves", numWaves );
        waterShader.setFloat( "time", currentFrame );
        waterShader.setFloat( "amplitude", amplitude );
        waterShader.setFloat( "frequency", frequency );
        waterShader.setFloat( "speed", speed );
        waterShader.setFloat( "ADecay", amplDecay );
        waterShader.setFloat( "wIncrease", waveLenIncrease );
        waterShader.setFloat( "kFactor", k );
        glm::vec3 camPos = cam.getPosition();
        waterShader.setVec3( "viewPos", camPos );
        waterShader.setVec3( "fogColor", fogColor );
        waterShader.setFloat( "fogStart", fogStart );
        waterShader.setFloat( "fogEnd", fogEnd );
        waterShader.setFloat( "gamma", gammaCorrection );
        waterShader.setFloat( "ambientStrength", ambient );
        waterShader.setFloat( "shininess", shininess );
        waterShader.setFloat( "fresnelStrength", fresnel );

        water.draw( waterShader );

        glDepthFunc( GL_LEQUAL );
        view = glm::mat4( glm::mat3( cam.getViewMat() ) ); // remove translation from the view matrix
        skyboxShader.activate();
        skyboxShader.setMat4( "view", view );
        skyboxShader.setMat4( "projection", projection );
        skyboxShader.setVec3( "fogColor", fogColor );
        skyboxShader.setFloat( "fogHeight", fogHeight );
        skyboxShader.setFloat( "gamma", gammaCorrection );

        glBindVertexArray( skyboxVAO );
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_CUBE_MAP, skyTextID );
        glDrawArrays( GL_TRIANGLES, 0, 36 );
        glDepthFunc( GL_LESS );

        renderText( "Number of waves : " + std::to_string( numWaves ) + "\nAmplitude : " + std::to_string( amplitude ) + "\nFrequency : " + std::to_string( frequency ) + "\nSpeed : " + std::to_string( speed ) + "\nAmplitude Decay : " + std::to_string( amplDecay ) + "\nWave Length Increase : " + std::to_string( waveLenIncrease ) + "\nK Factor : " + std::to_string( k ),
                    textShader, W_WIDTH * 0.01f, W_HEIGHT * 0.9f, 0.08f, textColor );
        renderText( "Ambient Strength : " + std::to_string( ambient ) + "\nShininess : " + std::to_string( shininess ) + "\nFresnel Strength : " + std::to_string( fresnel ) + "\nGamma Correction : " + std::to_string( gammaCorrection ) + "\nFog Start : " + std::to_string( fogStart ) + "\nFog End : " + std::to_string( fogEnd ) + "\nFog Height : " + std::to_string( fogHeight ),
                    textShader, W_WIDTH * 0.85f, W_HEIGHT * 0.9f, 0.08f, textColor );
        renderText( "FPS : " + std::to_string( (int)avgFPS / countFPS ), textShader, W_WIDTH * 0.9f, W_HEIGHT * 0.01f, 0.08f, textColor );
        renderText( "Current position : " + std::to_string( camPos.x ) + " " + std::to_string( camPos.y ) + " " + std::to_string( camPos.z ), textShader, W_WIDTH * 0.01f, W_HEIGHT * 0.01f, 0.08f, textColor );

        glfwSwapBuffers( window );
        glfwPollEvents();
    }
    glCheckError();
    glDeleteBuffers( 1, &skyboxVBO );
    glDeleteVertexArrays( 1, &skyboxVAO );
    glfwDestroyWindow( window );
    glfwTerminate();
    return 0;
}