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
	private:
		static VertexBuffer m_VBO;
		static std::unique_ptr<VertexArray> m_VAO;
		static std::shared_ptr<ShaderProgram> m_shader;
		static GLuint m_framebuffer, m_textureColorbuffer, m_rbo;

		static glm::mat3 m_kernel;
		static glm::mat3 m_filter;
		static float m_texelSize;
	};
}