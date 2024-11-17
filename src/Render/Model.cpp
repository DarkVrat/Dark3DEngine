#include "Model.h"
#include <map>
#include "../Manager/TextureLoader.h"

std::string directory;
std::map<std::string, std::shared_ptr<Render::Texture2D>> mapTextures;

namespace Render
{
	Model::Model(const std::string& path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return;
		}
		directory = path.substr(0, path.find_last_of('/'));

		processNode(scene->mRootNode, scene);
	}

	void Model::Draw(std::shared_ptr<ShaderProgram>& shader)
	{
		if (m_textures.diffuse != nullptr)
		{
			glActiveTexture(GL_TEXTURE0);
			shader->setInt("material.texture_diffuse1", 0);
			m_textures.diffuse->bind();

			if (m_textures.specular != nullptr)
			{
				glActiveTexture(GL_TEXTURE1);
				shader->setInt("material.texture_specular1", 1);
				m_textures.specular->bind();
			}

			if (m_textures.normal != nullptr)
			{
				glActiveTexture(GL_TEXTURE2);
				shader->setInt("material.texture_normal1", 2);
				m_textures.normal->bind();
			}

			if (m_textures.height != nullptr)
			{
				glActiveTexture(GL_TEXTURE3);
				shader->setInt("material.texture_height1", 3);
				m_textures.height->bind();
			}

			shader->setFloat("material.shininess", 128.f);
		}

		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}

	void Model::DrawInctanced(std::shared_ptr<ShaderProgram>& shader)
	{
		shader->use();
		if (m_textures.diffuse != nullptr)
		{
			glActiveTexture(GL_TEXTURE0);
			shader->setInt("material.texture_diffuse1", 0);
			m_textures.diffuse->bind();

			if (m_textures.specular != nullptr)
			{
				glActiveTexture(GL_TEXTURE1);
				shader->setInt("material.texture_specular1", 1);
				m_textures.specular->bind();
			}

			if (m_textures.normal != nullptr)
			{
				glActiveTexture(GL_TEXTURE2);
				shader->setInt("material.texture_normal1", 2);
				m_textures.normal->bind();
			}

			if (m_textures.height != nullptr)
			{
				glActiveTexture(GL_TEXTURE3);
				shader->setInt("material.texture_height1", 3);
				m_textures.height->bind();
			}
		}

		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			meshes[i].getVAO().bind();
			meshes[i].getEBO().bind();
			glDrawElementsInstanced(GL_TRIANGLES, meshes[i].getIndicesCount(), GL_UNSIGNED_INT, 0, m_inctenceCount);
		}
	}

	void Model::inctancedData(const std::vector<glm::mat4>& data)
	{
		m_VBOInctanced = std::make_unique<VertexBuffer>();
		m_VBOInctanced->initStatic(data.data(), data.size() * sizeof(glm::mat4));
		m_inctenceCount = data.size();

		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			VertexArray& VAO = meshes[i].getVAO();
			VAO.bind();
			// vertex attributes
			VAO.addBuffer(*m_VBOInctanced, 3, 4, sizeof(glm::mat4));
			VAO.setDivisor(3, 1);
			VAO.addBuffer(*m_VBOInctanced, 4, 4, sizeof(glm::mat4), 4);
			VAO.setDivisor(4, 1);
			VAO.addBuffer(*m_VBOInctanced, 5, 4, sizeof(glm::mat4), 2 * 4);
			VAO.setDivisor(5, 1);
			VAO.addBuffer(*m_VBOInctanced, 6, 4, sizeof(glm::mat4), 3 * 4);
			VAO.setDivisor(6, 1);
			VAO.unbind();
		}
	}

	void Model::processNode(aiNode* node, const aiScene* scene)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		for (unsigned int i = 0; i < node->mNumChildren ; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			
			vertex.Position.x = mesh->mVertices[i].x;
			vertex.Position.y = mesh->mVertices[i].y;
			vertex.Position.z = mesh->mVertices[i].z;
			vertex.Normal.x = mesh->mNormals[i].x;
			vertex.Normal.y = mesh->mNormals[i].y;
			vertex.Normal.z = mesh->mNormals[i].z;
			vertex.Tangent.x = mesh->mTangents[i].x;
			vertex.Tangent.y = mesh->mTangents[i].y;
			vertex.Tangent.z = mesh->mTangents[i].z;
			vertex.BiTangent.x = mesh->mBitangents[i].x;
			vertex.BiTangent.y = mesh->mBitangents[i].y;
			vertex.BiTangent.z = mesh->mBitangents[i].z;


			if (mesh->mTextureCoords[0])
			{
				vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
				vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);

			vertices.push_back(vertex);
		}
		
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
			std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
		}

		return Mesh(vertices, indices, textures);
	}

	std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
	{
		std::vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			std::string stdStr = std::string(str.C_Str());
			Texture texture;
			auto it = mapTextures.find(stdStr);
			if (it == mapTextures.end())
			{
				texture.texture = Managers::TextureLoader::loadTexture(directory + "/" + stdStr);
				mapTextures.emplace(stdStr, texture.texture);
			}
			else
			{
				texture.texture = it->second;
			}
			texture.type = typeName;
			textures.push_back(texture);
		}
		return textures;
	}
}
