#include "DeferredShading.h"

#include <iostream>
#include <glad/glad.h>
#include "../Manager/ConfigManager.h"
#include "../Manager/ResourceManager.h"

Render::VertexBuffer Render::DeferredShading::m_VBO;
std::unique_ptr<Render::VertexArray> Render::DeferredShading::m_VAO;
std::shared_ptr<Render::ShaderProgram> Render::DeferredShading::m_shader;

GLuint Render::DeferredShading::m_gBuffer;
GLuint Render::DeferredShading::m_gPosition;
GLuint Render::DeferredShading::m_gNormal;
GLuint Render::DeferredShading::m_gAlbedoSpec;
GLuint Render::DeferredShading::m_rboDepth;

namespace Render
{
	void DeferredShading::init()
	{
        glm::ivec2 windowSize = Managers::ConfigManager::getWindowSize();

        float quadVertices[] = {
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };

        m_VAO = std::make_unique<VertexArray>();
        m_VBO.initStatic(quadVertices, sizeof(quadVertices));
        m_VAO->addBuffer(m_VBO, 0, 2, 4 * sizeof(float), 0);
        m_VAO->addBuffer(m_VBO, 1, 2, 4 * sizeof(float), 2);

        m_shader = Managers::ResourceManager::getShader("light_for_DF");
        m_shader->use();
        m_shader->setInt("gPosition", 0);
        m_shader->setInt("gNormal", 1);
        m_shader->setInt("gAlbedoSpec", 2);

        // G-buffer
        glGenFramebuffers(1, &m_gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);

        // Position buffer
        glGenTextures(1, &m_gPosition);
        glBindTexture(GL_TEXTURE_2D, m_gPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowSize.x, windowSize.y, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gPosition, 0);

        // Normal buffer
        glGenTextures(1, &m_gNormal);
        glBindTexture(GL_TEXTURE_2D, m_gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowSize.x, windowSize.y, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gNormal, 0);

        // Albedo + Specular buffer
        glGenTextures(1, &m_gAlbedoSpec);
        glBindTexture(GL_TEXTURE_2D, m_gAlbedoSpec);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowSize.x, windowSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gAlbedoSpec, 0);

        glGenRenderbuffers(1, &m_rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, m_rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowSize.x, windowSize.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rboDepth);

        // Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
        GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);

        // Check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: G-buffer is not complete!" << std::endl;
         
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void DeferredShading::bind()
	{
        glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glm::ivec2 windowSize = Managers::ConfigManager::getWindowSize();
        glViewport(0, 0, windowSize.x, windowSize.y);
	}

	void DeferredShading::terminate()
	{
        glDeleteFramebuffers(1, &m_gBuffer);
        glDeleteRenderbuffers(1, &m_rboDepth);
        glDeleteTextures(1, &m_gPosition);
        glDeleteTextures(1, &m_gNormal);
        glDeleteTextures(1, &m_gAlbedoSpec);
	}

    void DeferredShading::Render()
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_gAlbedoSpec);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_shader->use();
        m_VAO->bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}
 