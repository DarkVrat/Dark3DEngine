#include "VertexArray.h"

namespace Render
{
	VertexArray::VertexArray()
	{
		glGenVertexArrays(1, &m_id);
	}

	VertexArray::VertexArray(VertexArray&& vertexArray) noexcept
	{
		m_id = vertexArray.m_id;
		vertexArray.m_id = 0;
	}

	VertexArray& VertexArray::operator=(VertexArray&& vertexArray) noexcept
	{
		if (m_id == vertexArray.m_id) return *this;

		m_id = vertexArray.m_id;
		vertexArray.m_id = 0;

		return *this;
	}

	VertexArray::~VertexArray() noexcept
	{
		glDeleteVertexArrays(1, &m_id);
	}

	void VertexArray::addBuffer(const VertexBuffer& vertexBuffer, GLuint layoutID, GLuint size, GLuint stride, GLenum type, GLvoid* pointer, bool divisor)
	{
		bind();
		vertexBuffer.bind();

		glVertexAttribPointer(layoutID, size, type, GL_FALSE, (size+stride) * sizeof(float), pointer);
		glEnableVertexAttribArray(layoutID);

		if (divisor) glVertexAttribDivisor(layoutID, 1);
	}

	void VertexArray::bind() const
	{
		glBindVertexArray(m_id);
	}

	void VertexArray::unbind()
	{
		glBindVertexArray(0);
	}
}