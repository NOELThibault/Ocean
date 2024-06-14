#ifndef MODEL_HPP
#define MODEL_HPP

#include <string>
#include <vector>
#include <assimp/scene.h>
#include <shader.hpp>
#include <mesh.hpp>

class Model
{
    public :
        std::vector< Texture > textureLoaded;

        Model( std::string path, bool gamma = false ) noexcept;
        void draw( Shader & shader ) const noexcept;

    private :
        std::vector< Mesh > meshes;
        std::string directory;
        bool gammaCorrection;

        void processNode( aiNode * node, const aiScene * scene ) noexcept;
        Mesh processMesh( aiMesh * mesh, const aiScene * scene ) noexcept;
        std::vector< Texture > loadMaterialTextures( aiMaterial * material, aiTextureType type, std::string typeName ) noexcept;
        unsigned int textureFromFile( const char * path, const std::string & directory, bool gamma ) const noexcept;
};

#endif