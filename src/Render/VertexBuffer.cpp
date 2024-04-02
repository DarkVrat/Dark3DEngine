#include "VertexBuffer.h"

namespace Render 
{
	VertexBuffer::VertexBuffer(VertexBuffer&& vertexBuffer) noexcept
	{
		m_id = vertexBuffer.m_id;
		m_type = vertexBuffer.m_type;
		vertexBuffer.m_id = 0;
		vertexBuffer.m_type = GL_ARRAY_BUFFER;
	}

	VertexBuffer& VertexBuffer::operator=(VertexBuffer&& vertexBuffer) noexcept
	{
		if (m_id == vertexBuffer.m_id) return *this;

		m_id = vertexBuffer.m_id;
		m_type = vertexBuffer.m_type;
		vertexBuffer.m_id = 0;
		vertexBuffer.m_type = GL_ARRAY_BUFFER;

		return *this;
	}

	VertexBuffer::~VertexBuffer() noexcept
	{
		glDeleteBuffers(1, &m_id);
	}

	void VertexBuffer::initStatic(const void* data, const unsigned& size)
	{
		glGenBuffers(1, &m_id);
		glBindBuffer(m_type, m_id);
		glBufferData(m_type, size, data, GL_STATIC_DRAW);
	}

	void VertexBuffer::initDynamic(const void* data, const unsigned& size)
	{
		glGenBuffers(1, &m_id);
		glBindBuffer(m_type, m_id);
		glBufferData(m_type, size, data, GL_DYNAMIC_DRAW);
	}

	void VertexBuffer::update(const void* data, const unsigned& size) const
	{
		glBindBuffer(m_type, m_id);
		glBufferData(m_type, size, data, GL_DYNAMIC_DRAW);
	}

	void VertexBuffer::bind() const
	{
		glBindBuffer(m_type, m_id);
	}

	void VertexBuffer::unbind() const
	{
		glBindBuffer(m_type, 0);
	}
}
