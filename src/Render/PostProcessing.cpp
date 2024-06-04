#include "PostProcessing.h"

#include <iostream>
#include <glad/glad.h>
#include "../Manager/ConfigManager.h"
#include "../Manager/ResourceManager.h"

Render::VertexBuffer Render::PostProcessing::m_VBO;
std::unique_ptr<Render::VertexArray> Render::PostProcessing::m_VAO;
std::shared_ptr<Render::ShaderProgram> Render::PostProcessing::m_shader = nullptr;
GLuint Render::PostProcessing::m_framebuffer;
GLuint Render::PostProcessing::m_textureColorBufferMultiSampled;
GLuint Render::PostProcessing::m_rbo;
GLuint Render::PostProcessing::m_intermediateFBO;
GLuint Render::PostProcessing::m_screenTexture;

glm::mat3 Render::PostProcessing::m_kernel = glm::mat3(0,0,0,0,1,0,0,0,0);
glm::mat3 Render::PostProcessing::m_filter = glm::mat3(1);
float Render::PostProcessing::m_texelSize = 1./1000.f;

namespace Render
{
	void PostProcessing::init()
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

        glGenFramebuffers(1, &m_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

        glGenTextures(1, &m_textureColorBufferMultiSampled);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_textureColorBufferMultiSampled);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Managers::ConfigManager::getSamples(), GL_RGB, windowSize.x, windowSize.y, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_textureColorBufferMultiSampled, 0);

        glGenRenderbuffers(1, &m_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, Managers::ConfigManager::getSamples(), GL_DEPTH24_STENCIL8, windowSize.x, windowSize.y);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glGenFramebuffers(1, &m_intermediateFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_intermediateFBO);

        glGenTextures(1, &m_screenTexture);
        glBindTexture(GL_TEXTURE_2D, m_screenTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowSize.x, windowSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_screenTexture, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_shader = Managers::ResourceManager::getShader("post_process_shader");
        m_shader->use();
        m_shader->setMatrix3("kernel", m_kernel);
        m_shader->setMatrix3("filter", m_filter);
        m_shader->setFloat("texelSize", m_texelSize);
	}

    void PostProcessing::bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void PostProcessing::render()
    {
        glm::ivec2 windowSize = Managers::ConfigManager::getWindowSize();
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_framebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_intermediateFBO);
        glBlitFramebuffer(0, 0, windowSize.x, windowSize.y, 0, 0, windowSize.x, windowSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        m_shader->use();
        m_VAO->bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_screenTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void PostProcessing::terminate()
    {
        glDeleteFramebuffers(1, &m_framebuffer);
        glDeleteRenderbuffers(1, &m_rbo);
    }

    void PostProcessing::setKernel(const glm::mat3& kernel)
    {
        m_kernel = kernel;
        if (m_shader != nullptr)
        {
            m_shader->use();
            m_shader->setMatrix3("kernel", m_kernel);
        }
    }

    void PostProcessing::setFilter(const glm::mat3& filter)
    {
        m_filter = filter;
        if (m_shader != nullptr)
        {
            m_shader->use();
            m_shader->setMatrix3("filter", m_filter);
        }
    }

    void PostProcessing::setTexelSize(const float& texelSize)
    {
        m_texelSize = texelSize;
        if (m_shader != nullptr)
        {
            m_shader->use();
            m_shader->setFloat("texelSize", m_texelSize);
        }
    }
}
