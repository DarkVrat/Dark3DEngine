#pragma once

#include <memory>
#include "VertexArray.h"
#include "ShaderProgram.h"

namespace Render
{
	class PostProcessing
	{
	public:
		static void init();
		static void bind();
		static void render();
		static void terminate();

		static void setKernel(const glm::mat3& kernel);
		static void setFilter(const glm::mat3& filter);
		static void setTexelSize(const float& texelSize);
		static void setGamma(const float& gamma);
		static void setExposure(const float& exposure);
		static void setAmountBloom(const int& amountBloom);
		static void setIntensity(const float& intensity);
	private:
		static VertexBuffer m_VBO;
		static std::unique_ptr<VertexArray> m_VAO;
		static std::shared_ptr<ShaderProgram> m_shader;
		static GLuint m_framebuffer;
		static GLuint m_textureColorBufferMultiSampled;
		static GLuint m_rbo;
		static GLuint m_intermediateFBO;
		static GLuint m_screenTexture;

		static GLuint m_pingpongFBO[2];
		static GLuint m_pingpongColorbuffers[2];
		static std::shared_ptr<ShaderProgram> m_bloomExtractShader;
		static std::shared_ptr<ShaderProgram> m_bloomBlurShader;
		static int m_amountBloom;
		static float m_intensity;

		static glm::mat3 m_kernel;
		static glm::mat3 m_filter;
		static float m_texelSize;
		static float m_gamma;
		static float m_exposure;
	};
}
