#pragma once

#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Render
{
    struct simpleTexture
    {
        std::shared_ptr<Texture2D> diffuse = nullptr;
        std::shared_ptr<Texture2D> specular = nullptr;
        std::shared_ptr<Texture2D> normal = nullptr;
    };

	class Model
	{
    public:
        Model(const std::string& path);
        void Draw(std::shared_ptr<ShaderProgram>& shader);

        simpleTexture& getSTexture() { return m_textures; };
    private:
        std::vector<Mesh> meshes;
        std::string directory;
        simpleTexture m_textures;

        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	};
}