#pragma once

#include <glad/glad.h>

namespace Render
{
	class VertexBuffer
	{
	public:
		VertexBuffer(GLenum type = GL_ARRAY_BUFFER) :m_id(0), m_type(type) {};
		VertexBuffer(const VertexBuffer& vertexBuffer) = delete;
		VertexBuffer(VertexBuffer&& vertexBuffer) noexcept;
		VertexBuffer& operator=(const VertexBuffer& vertexBuffer) = delete;
		VertexBuffer& operator=(VertexBuffer&& vertexBuffer) noexcept;
		~VertexBuffer() noexcept;

		void initStatic(const void* data, const unsigned& size);
		void initDynamic(const void* data, const unsigned& size);
		void update(const void* data, const unsigned& size) const;
		void bind() const;
		void unbind() const;
		GLuint m_id;
	private:
		//GL_ARRAY_BUFFER = 34962
		//GL_ELEMENT_ARRAY_BUFFER = 34963
		GLenum m_type;
		
	};
}