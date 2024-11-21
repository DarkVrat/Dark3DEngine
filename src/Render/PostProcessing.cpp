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

GLuint Render::PostProcessing::m_pingpongFBO[2];
GLuint Render::PostProcessing::m_pingpongColorbuffers[2];
std::shared_ptr<Render::ShaderProgram> Render::PostProcessing::m_bloomExtractShader;
std::shared_ptr<Render::ShaderProgram> Render::PostProcessing::m_bloomBlurShader;
int Render::PostProcessing::m_amountBloom = 5; 
float Render::PostProcessing::m_intensity = 3.f;

glm::mat3 Render::PostProcessing::m_kernel = glm::mat3(0,0,0,0,1,0,0,0,0);
glm::mat3 Render::PostProcessing::m_filter = glm::mat3(1);
float Render::PostProcessing::m_texelSize = 1.f / 1000.f;
float Render::PostProcessing::m_gamma = 0.675f;
float Render::PostProcessing::m_exposure = 2.0f;

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
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Managers::ConfigManager::getSamples(), GL_RGBA16F, windowSize.x, windowSize.y, GL_TRUE);
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowSize.x, windowSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
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
        m_shader->setFloat("gamma", m_gamma);
        m_shader->setFloat("exposure", m_exposure);
        m_shader->setInt("screenTexture", 0);
        m_shader->setInt("bloomBlur", 1);

        glGenFramebuffers(2, m_pingpongFBO);
        glGenTextures(2, m_pingpongColorbuffers);

        for (unsigned int i = 0; i < 2; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFBO[i]);
            glBindTexture(GL_TEXTURE_2D, m_pingpongColorbuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowSize.x, windowSize.y, 0, GL_RGBA, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pingpongColorbuffers[i], 0);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "ERROR::FRAMEBUFFER:: Pingpong framebuffer is not complete! " << i <<  std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_bloomExtractShader = Managers::ResourceManager::getShader("bloom_extract_shader");
        m_bloomBlurShader = Managers::ResourceManager::getShader("bloom_blur_shader");

        m_bloomExtractShader->use();
        m_bloomExtractShader->setFloat("intensity", m_intensity);
    } 

    void PostProcessing::bind()
    {
        glm::ivec2 windowSize = Managers::ConfigManager::getWindowSize();
        glViewport(0, 0, windowSize.x, windowSize.y);
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void PostProcessing::render()
    {
        glDisable(GL_DEPTH_TEST);

        glm::ivec2 windowSize = Managers::ConfigManager::getWindowSize();
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_framebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_intermediateFBO);
        glBlitFramebuffer(0, 0, windowSize.x, windowSize.y, 0, 0, windowSize.x, windowSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        m_bloomExtractShader->use();
        glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFBO[0]);
        glClear(GL_COLOR_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_screenTexture);
        m_VAO->bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);

        bool horizontal = true, first_iteration = true;
        for (int i = 0; i < m_amountBloom; i++)
        {
            m_bloomBlurShader->use();
            m_bloomBlurShader->setInt("horizontal", true);
            glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFBO[1]);
            glClear(GL_COLOR_BUFFER_BIT);
            glActiveTexture(GL_TEXTURE0); 
            glBindTexture(GL_TEXTURE_2D, m_pingpongColorbuffers[0]);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            
            m_bloomBlurShader->setInt("horizontal", false);
            glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFBO[0]);
            glClear(GL_COLOR_BUFFER_BIT);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_pingpongColorbuffers[1]);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_shader->use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_screenTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_pingpongColorbuffers[0]);
        m_VAO->bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void PostProcessing::terminate()
    {
        glDeleteFramebuffers(1, &m_framebuffer);
        glDeleteRenderbuffers(1, &m_rbo);

        glDeleteFramebuffers(2, m_pingpongFBO);
        glDeleteTextures(2, m_pingpongColorbuffers);
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

    void PostProcessing::setGamma(const float& gamma)
    {
        m_gamma = gamma;
        if (m_shader != nullptr)
        {
            m_shader->use();
            m_shader->setFloat("gamma", m_gamma);
        }
    }

    void PostProcessing::setExposure(const float& exposure)
    {
        m_exposure = exposure;
        if (m_shader != nullptr)
        {
            m_shader->use();
            m_shader->setFloat("exposure", m_exposure);
        }
    }

    void PostProcessing::setAmountBloom(const int& amountBloom)
    {
        m_amountBloom = amountBloom;
    }

    void PostProcessing::setIntensity(const float& intensity)
    {
        m_intensity = intensity;
        if (m_bloomExtractShader != nullptr)
        {
            m_bloomExtractShader->use();
            m_bloomExtractShader->setFloat("intensity", m_intensity);
        }
    }
}
