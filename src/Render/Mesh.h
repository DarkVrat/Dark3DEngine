#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "VertexArray.h"

namespace Render
{
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
        glm::vec3 Tangent;
        glm::vec3 BiTangent;
    };

    struct Texture {
        std::shared_ptr<Texture2D> texture;
        std::string type;
    };

    class Mesh {
    public:
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
        void Draw(std::shared_ptr<ShaderProgram> shader);
        VertexArray& getVAO() { return m_VAO; }
        VertexBuffer& getEBO() { return m_EBO; }
        size_t getIndicesCount() { return m_indicesCount; }
    private:
        std::vector<Texture> m_textures;
        size_t m_indicesCount;

        VertexArray m_VAO;
        VertexBuffer m_EBO = VertexBuffer(GL_ELEMENT_ARRAY_BUFFER);
        VertexBuffer m_VBO;
    };
}