#include "Mesh.h"

#include <string>

namespace Render
{
	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
	{
		this->m_textures = textures;

        m_indicesCount = indices.size();
        m_EBO.initStatic(indices.data(), indices.size()*sizeof(unsigned int));
        m_VBO.initStatic(vertices.data(), vertices.size()*sizeof(Vertex));

        m_VAO.bind();
        m_VAO.addBuffer(m_VBO, 0, 3, sizeof(Vertex));
        m_VAO.addBuffer(m_VBO, 1, 3, sizeof(Vertex), 3);
        m_VAO.addBuffer(m_VBO, 2, 2, sizeof(Vertex), 6);
		m_VAO.unbind();
	}

	void Mesh::Draw(std::shared_ptr<ShaderProgram> shader)
	{
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        for (unsigned int i = 0; i < m_textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            std::string number;
            std::string name = m_textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);

            shader->setInt(("material." + name + number).c_str(), i);
            m_textures[i].texture->bind();
        }
        glActiveTexture(GL_TEXTURE0);

        m_VAO.bind();
        m_EBO.bind();
        glDrawElements(GL_TRIANGLES, m_indicesCount, GL_UNSIGNED_INT, 0);
        m_VAO.unbind();
	}
}
