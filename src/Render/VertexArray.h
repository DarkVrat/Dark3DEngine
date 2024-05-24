#pragma once

#include <glad/glad.h>
#include "VertexBuffer.h"

namespace Render
{
	class VertexArray
	{
	public:
		VertexArray();
		VertexArray(const VertexArray& vertexArray) = delete;
		VertexArray(VertexArray&& vertexArray) noexcept;
		VertexArray& operator=(const VertexArray& vertexArray) = delete;
		VertexArray& operator=(VertexArray&& vertexArray) noexcept;
		~VertexArray() noexcept;

		void addBuffer(const VertexBuffer& vertexBuffer, GLuint layoutID, GLuint sizeData, GLuint sizeFullData, GLuint pointer = 0, GLenum type = GL_FLOAT);
		void setDivisor(GLuint layoutID, int divisor);
		void bind() const;

		static void unbind();

	private:
		GLuint m_id = 0;
	};
}