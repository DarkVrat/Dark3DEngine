#include "MaterialClass.h"

#define DIFFUSE_TEXTURE GL_TEXTURE0
#define SPECULAR_TEXTURE GL_TEXTURE1

namespace Render
{
	MaterialClass::MaterialClass(std::unique_ptr<Texture2D>&& diffuse, float shiniess) :m_shiniess(shiniess), m_diffuse(std::move(diffuse))	{}

    MaterialClass::MaterialClass(MaterialClass&& material) noexcept
	{
		m_diffuse = std::move(material.m_diffuse);
		m_specular = std::move(material.m_specular);
		m_shiniess = material.m_shiniess;
	}

	MaterialClass& MaterialClass::operator=(MaterialClass&& material) noexcept
	{
		if (material.m_diffuse == m_diffuse)
			return *this;

		m_diffuse = std::move(material.m_diffuse);
		m_specular = std::move(material.m_specular);
		m_shiniess = material.m_shiniess;

		return *this;
	}

	void MaterialClass::setSpecular(std::unique_ptr<Texture2D>&& specular)
	{
		m_specular = std::move(specular);
	}

	void MaterialClass::bind() const noexcept
	{
		bindHelp(DIFFUSE_TEXTURE, m_diffuse);
		if(m_shiniess) bindHelp(SPECULAR_TEXTURE, m_specular);
	}

	inline void MaterialClass::bindHelp(GLenum type, const std::unique_ptr<Texture2D>& texture) noexcept
	{
		glActiveTexture(type);
		texture->bind();
	}
}
