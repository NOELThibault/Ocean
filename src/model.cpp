#include <model.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>
#include <mesh.hpp>

Model::Model( std::string path, bool gamma ) noexcept : gammaCorrection( gamma )
{
    Assimp::Importer importer;
    const aiScene * scene = importer.ReadFile( path, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_FlipUVs );
    if( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode )
    {
        std::cerr << "ASSIMP ERROR - " << importer.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr( 0, path.find_last_of( '/' ) );
    processNode( scene->mRootNode, scene );
}

void Model::draw( Shader & shader ) const noexcept
{
    for( int i = 0; i < meshes.size(); i++ )
    {
        meshes[i].draw( shader );
    }
}

void Model::processNode( aiNode * node, const aiScene * scene ) noexcept
{
    // process all the node's meshes (if any)
    for( unsigned int i = 0; i < node->mNumMeshes; i++ )
    {
        aiMesh * mesh = scene->mMeshes[ node->mMeshes[i] ]; 
        meshes.push_back( processMesh( mesh, scene ) );	
    }
    // then do the same for each of its children
    for( unsigned int i = 0; i < node->mNumChildren; i++ )
    {
        processNode( node->mChildren[i], scene );
    }
}

Mesh Model::processMesh( aiMesh * mesh, const aiScene * scene ) noexcept
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for( int i = 0; i < mesh->mNumVertices; i++ )
    {
        Vertex vertex;
        glm::vec3 placeHolder;
        placeHolder.x = mesh->mVertices[i].x;
        placeHolder.y = mesh->mVertices[i].y;
        placeHolder.z = mesh->mVertices[i].z;
        vertex.pos = placeHolder;
        if( mesh->HasNormals() )
        {
            placeHolder.x = mesh->mNormals[i].x;
            placeHolder.y = mesh->mNormals[i].y;
            placeHolder.z = mesh->mNormals[i].z;
            vertex.normal = placeHolder;
        }
        else
        {
            vertex.normal = glm::vec3( 0.0f );
        }
        if( mesh->mTextureCoords[0] )
        {
            glm::vec2 textCoord;
            textCoord.x = mesh->mTextureCoords[0][i].x;
            textCoord.y = mesh->mTextureCoords[0][i].y;
            vertex.uv = textCoord;
            placeHolder.x = mesh->mTangents[i].x;
            placeHolder.y = mesh->mTangents[i].y;
            placeHolder.z = mesh->mTangents[i].z;
            vertex.tangent = placeHolder;
            placeHolder.x = mesh->mBitangents[i].x;
            placeHolder.y = mesh->mBitangents[i].y;
            placeHolder.z = mesh->mBitangents[i].z;
            vertex.bitangent = placeHolder;
        }
        else
        {
            vertex.uv = glm::vec2( 0.0f, 0.0f );
        }
        vertices.push_back( vertex );
    }
    for( int i = 0; i < mesh->mNumFaces; i++ )
    {
        aiFace face = mesh->mFaces[i];
        for( int j = 0; j < face.mNumIndices; j++ )
        {
            indices.push_back( face.mIndices[j] );
        }
    }
    aiMaterial * material = scene->mMaterials[ mesh->mMaterialIndex ];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    std::vector< Texture > diffuseMaps = loadMaterialTextures( material, aiTextureType_DIFFUSE, "texture_diffuse" );
    textures.insert( textures.end(), diffuseMaps.begin(), diffuseMaps.end() );
    // 2. specular maps
    std::vector< Texture > specularMaps = loadMaterialTextures( material, aiTextureType_SPECULAR, "texture_specular" );
    textures.insert( textures.end(), specularMaps.begin(), specularMaps.end() );
    // 3. normal maps
    std::vector< Texture > normalMaps = loadMaterialTextures( material, aiTextureType_HEIGHT, "texture_normal" );
    textures.insert( textures.end(), normalMaps.begin(), normalMaps.end() );
    // 4. height maps
    std::vector<Texture> heightMaps = loadMaterialTextures( material, aiTextureType_AMBIENT, "texture_height" );
    textures.insert( textures.end(), heightMaps.begin(), heightMaps.end() );
    
    return Mesh( vertices, indices, textures );
}

std::vector< Texture > Model::loadMaterialTextures( aiMaterial * material, aiTextureType type, std::string typeName ) noexcept
{
    std::vector< Texture > textures;
    for( int i = 0; i < material->GetTextureCount( type ); i++ )
    {
        aiString str;
        material->GetTexture( type, i, &str );
        bool skip = false;
        for( int j = 0; j < textureLoaded.size(); j++ )
        {
            if( std::strcmp( textureLoaded[j].path.data(), str.C_Str() ) == 0 )
            {
                textures.push_back( textureLoaded[j] );
                skip = true; 
                break;
            }
        }
        if( !skip )
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = textureFromFile( str.C_Str(), directory, false );
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back( texture );
            textureLoaded.push_back( texture );
        }
    }
    return textures;
}

unsigned int Model::textureFromFile( const char * path, const std::string & directory, bool gamma ) const noexcept
{
    std::string filename = std::string( path );
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures( 1, &textureID );

    int width, height, nrComponents;
    unsigned char * data = stbi_load( filename.c_str(), &width, &height, &nrComponents, 0 );
    if( data )
    {
        GLenum format;
        if ( nrComponents == 1 )
            format = GL_RED;
        else if ( nrComponents == 3 )
            format = GL_RGB;
        else if ( nrComponents == 4 )
            format = GL_RGBA;

        glBindTexture( GL_TEXTURE_2D, textureID );
        glTexImage2D( GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data );
        glGenerateMipmap( GL_TEXTURE_2D );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
    }
    stbi_image_free( data );

    return textureID;
}