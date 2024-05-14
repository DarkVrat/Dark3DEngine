#include "SkyboxRender.h"

#include "stb_image.h"

namespace Render
{
	SkyboxRender::SkyboxRender(const std::vector<std::string>& paths)
	{
        float skyboxVertices[] = {        
            -1.0f,  1.0f, -1.0f,    -1.0f, -1.0f, -1.0f,     1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,     1.0f,  1.0f, -1.0f,    -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,	-1.0f, -1.0f, -1.0f,	-1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,	-1.0f,  1.0f,  1.0f,	-1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,	 1.0f, -1.0f,  1.0f,	 1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,	 1.0f,  1.0f, -1.0f,	 1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,	-1.0f,  1.0f,  1.0f,	 1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,	 1.0f, -1.0f,  1.0f,	-1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,	 1.0f,  1.0f, -1.0f,	 1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,	-1.0f,  1.0f,  1.0f,	-1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,	-1.0f, -1.0f,  1.0f,	 1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,	-1.0f, -1.0f,  1.0f,	 1.0f, -1.0f,  1.0f
        };

		m_VBO.initStatic(skyboxVertices, sizeof(skyboxVertices));
		m_VAO.addBuffer(m_VBO, 0, 3, 3);

        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);

        int width, height, nrChannels;
        for (unsigned int i = 0; i < paths.size(); i++)
        {
            unsigned char* data = stbi_load(paths[i].c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else
            {
                std::cout << "Cubemap texture failed to load at path: " << paths[i] << std::endl;
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	SkyboxRender::SkyboxRender(SkyboxRender&& right) noexcept :m_VAO(std::move(right.m_VAO)), m_VBO(std::move(right.m_VBO)), m_shader(right.m_shader), m_textureID(right.m_textureID)
	{
		right.m_textureID = 0;
	}

	SkyboxRender& SkyboxRender::operator=(SkyboxRender&& right) noexcept
	{
		if (right.m_textureID == m_textureID)
			return *this;

		m_VAO = std::move(right.m_VAO);
		m_VBO = std::move(right.m_VBO);
		m_shader = right.m_shader;
		m_textureID = right.m_textureID;
		right.m_textureID = 0;

        return *this;
	}

	void SkyboxRender::render()
	{
        glDepthFunc(GL_LEQUAL);
        m_shader->use();
		m_VAO.bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
        glDrawArrays(GL_TRIANGLES, 0, 36);
		m_VAO.unbind();
        glDepthFunc(GL_LESS);
	}
}
