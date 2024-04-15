#pragma once

#include "Texture2D.h"
#include <memory>

namespace Render
{
	class MaterialClass
	{
	public:
		MaterialClass(std::unique_ptr<Texture2D>&& diffuse, float shiniess);
		MaterialClass(const MaterialClass& material) = delete;
		MaterialClass(MaterialClass&& material) noexcept;
		MaterialClass& operator=(const MaterialClass& material) = delete;
		MaterialClass& operator=(MaterialClass&& material) noexcept;
		~MaterialClass() noexcept {};

		void setSpecular(std::unique_ptr<Texture2D>&& specular);
		void bind() const noexcept;
		inline float getShiniess() { return m_shiniess; };
	private:
		static void bindHelp(GLenum type, const std::unique_ptr<Texture2D>& texture) noexcept;

		std::unique_ptr<Texture2D> m_diffuse;
		std::unique_ptr<Texture2D> m_specular = nullptr;
		float m_shiniess;
	};
}