#pragma once

#include <glad/glad.h>
#include <iostream>

namespace Render
{
	class Texture2D
	{
	public:
		Texture2D(const std::string& TexturePath);
		Texture2D(const Texture2D& OtherTexture) = delete;
		Texture2D(Texture2D&& OtherTexture) noexcept;
		Texture2D& operator=(const Texture2D& OtherTexture) = delete;
		Texture2D& operator=(Texture2D&& OtherTexture) noexcept;
		~Texture2D() noexcept;

		void setParam(GLuint WrapS, GLuint WrapT, GLuint MinFilter, GLuint MagFilter) const noexcept;
		void setActiveId(GLuint id) noexcept;

		void bind() const noexcept;

		GLuint m_id;
		GLuint m_activeId = 0;
		int m_width;
		int m_height;
		int m_channels;
	private:
		
	};
}