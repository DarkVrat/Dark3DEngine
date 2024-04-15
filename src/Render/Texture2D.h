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

		void bind() const noexcept;

	private:
		GLuint m_id;
		int m_width;
		int m_height;
		int m_channels;
	};
}