#include <hud.hpp>
#include <exception>
#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

const float HUD::rectangleVertices[ 8 ] = 
{
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 0.0f
};

const unsigned int HUD::rectangleIndices[ 6 ] =
{
    2, 3, 0,
    3, 1, 0
};

HUD::HUD( const int wWidth, const int wHeight, const char * fontPath )
{
    if( FT_Init_FreeType( &ft ) )
    {
        throw std::runtime_error( "Failed to initialize FreeType" );
    }
    if( FT_New_Face( ft, fontPath, 0, &face ) )
    {
        throw std::runtime_error( "Failed to load face" );
    }        
    if( loadFont( ft, face ) == -1 )
    {
        throw std::runtime_error( "Failed to load font" );
    }    
    
    screenProjection = glm::ortho( 0.0f, static_cast< float >( wWidth ), 0.0f, static_cast< float >( wHeight ) );
    textShader.activate();
    textShader.setMat4( "projection", screenProjection );

}    

void HUD::renderText( const std::string & text, float x, float y, float scale, glm::vec4 & color ) noexcept
{
    glDisable( GL_DEPTH_TEST );
    float ogX = x;
    textShader.activate();
    textShader.setVec4( "textColor", color );

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
    textShader.setMat4Array( "transforms", textTransforms, currentIndex );
    textShader.setIntArray( "letterMap", letterMap, currentIndex );
    glDrawArraysInstanced( GL_TRIANGLE_STRIP, 0, 4, currentIndex );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray( 0 );
    glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );
    glEnable( GL_DEPTH_TEST );
}    

int HUD::loadFont( FT_Library ft, FT_Face face ) noexcept
{
    FT_Set_Pixel_Sizes( face, 256, 256 );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    glGenTextures( 1, &textArray );
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D_ARRAY, textArray );
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

    glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

    FT_Done_Face( face );
    FT_Done_FreeType( ft );

    for( int i = 0; i < TEXT_MAX_BATCH; i++ )
    {
        textTransforms[i] = glm::mat4( 1.0f );
        letterMap[i] = 0;
    }    
    // Set up buffers
    glGenVertexArrays( 1, &textVAO );
    glGenBuffers( 1, &textVBO );
    glBindVertexArray( textVAO );
    glBindBuffer( GL_ARRAY_BUFFER, textVBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( rectangleVertices ), rectangleVertices, GL_STATIC_DRAW );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray( 0 );
    return 0;
}    

