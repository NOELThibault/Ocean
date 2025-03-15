#ifndef HUD_HPP
#define HUD_HPP

#include <string>
#include <glm/glm.hpp>
#include <shader/shader.hpp>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H

#define TEXT_MAX_BATCH 200

struct Character
{
    int characterId;
    glm::ivec2 size;
    glm::ivec2 bearing;
    unsigned int xAdvance;
};

class HUD
{
    public :
        HUD( const int wWidth, const int wHeight, const char * fontPath );
        HUD() = default;
        ~HUD() = default;
        
        int loadFont( FT_Library ft, FT_Face face ) noexcept;
        
        void renderText( const std::string & text, float x, float y, float scale, glm::vec4 & color ) noexcept;

    private :
        FT_Library ft;
        FT_Face face;
        
        static const float rectangleVertices[ 8 ];
        static const unsigned int rectangleIndices[ 6 ];

        glm::mat4 screenProjection;

        unsigned int textVAO, textVBO;    
        Shader textShader = { "../include/shader/text.vs", "../include/shader/text.fs" };

        unsigned int textArray;
        glm::mat4 textTransforms[ TEXT_MAX_BATCH ];
        int letterMap[ TEXT_MAX_BATCH ];
        std::map< char, Character > characters;
};

#endif