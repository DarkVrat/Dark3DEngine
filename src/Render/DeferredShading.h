#pragma once

#include <memory>
#include "VertexArray.h"
#include "ShaderProgram.h"

namespace Render
{
	class DeferredShading
	{
	public:
		static void init();
		static void bind();
		static void terminate();
		static void Render();
	private:
		static VertexBuffer m_VBO;
		static std::unique_ptr<VertexArray> m_VAO;
		static std::shared_ptr<ShaderProgram> m_shader;

		static GLuint m_gBuffer;
		static GLuint m_gPosition;
		static GLuint m_gNormal;
		static GLuint m_gAlbedoSpec;
		static GLuint m_rboDepth;
	};

}