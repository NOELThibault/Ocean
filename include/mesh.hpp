#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <shader.hpp>

#define MAX_BONE_INFLUENCE 4

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    int boneIDs[ MAX_BONE_INFLUENCE ];
    float weights[ MAX_BONE_INFLUENCE ];
};

struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh
{
    public :
        Mesh( std::vector< Vertex > vertices, std::vector< unsigned int > indices, std::vector< Texture > textures ) noexcept;
        void draw( Shader & shader ) const noexcept;
    
    private :
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;
        unsigned int vao, vbo, ebo;

};

#endif