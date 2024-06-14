#include <mesh.hpp>
#include <glad/glad.h>

Mesh::Mesh( std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures ) noexcept :
    vertices( vertices ), indices( indices ), textures( textures )
{
    glGenVertexArrays( 1, &vao );
    glGenBuffers( 1, &vbo );
    glGenBuffers( 1, &ebo );

    glBindVertexArray( vao );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );

    glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( Vertex ), &vertices[0], GL_STATIC_DRAW );  

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof( unsigned int ), &indices[0], GL_STATIC_DRAW );

    // vertex positions
    glEnableVertexAttribArray( 0 );	
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void*)0 );
    // vertex normals
    glEnableVertexAttribArray( 1 );	
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void*)offsetof( Vertex, normal ) );
    // vertex texture coords
    glEnableVertexAttribArray( 2 );	
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void*)offsetof( Vertex, uv ) );
    // vertex tangent
    glEnableVertexAttribArray( 3 );
    glVertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void*)offsetof( Vertex, tangent ) );
    // vertex bitangent
    glEnableVertexAttribArray( 4 );
    glVertexAttribPointer( 4, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void*)offsetof( Vertex, bitangent ) );
    // ids
    glEnableVertexAttribArray( 5 );
    glVertexAttribIPointer( 5, 4, GL_INT, sizeof( Vertex ), (void*)offsetof( Vertex, boneIDs ) );
    // weights
    glEnableVertexAttribArray( 6 );
    glVertexAttribPointer( 6, 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void*)offsetof( Vertex, weights ) );

    glBindVertexArray( 0 );
}

void Mesh::draw( Shader & shader ) const noexcept
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    for( int i = 0; i < textures.size(); i++ )
    {
        glActiveTexture( GL_TEXTURE0 + i ); // activate proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = textures[i].type;
        if( name == "texture_diffuse" )
        {
            number = std::to_string( diffuseNr );
            diffuseNr++;
        }
        else if( name == "texture_specular" )
        {
            number = std::to_string( specularNr );
            specularNr++;
        }
        else if( name == "texture_normal" )
        {
            number = std::to_string( normalNr );
            normalNr++;
        }
        else if( name == "texture_height" )
        {
            number = std::to_string( heightNr );
            heightNr++;
        }

        shader.setInt( ( "material." + name + number ).c_str(), i );
        glBindTexture( GL_TEXTURE_2D, textures[i].id );
    }
    glActiveTexture( GL_TEXTURE0 );

    glBindVertexArray( vao );
    glDrawElements( GL_TRIANGLES, static_cast<unsigned int>( indices.size() ), GL_UNSIGNED_INT, 0 );
    glBindVertexArray( 0 );
    glActiveTexture( GL_TEXTURE0 );
}