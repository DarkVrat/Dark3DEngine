#pragma once

#include <map>
#include <memory>
#include <iostream>
#include "../Render/Texture2D.h"

namespace Managers
{
	class TextureLoader
	{
	public:
		static std::shared_ptr<Render::Texture2D> loadTexture(const std::string& texturePath);
	private:
		static std::map<std::string, std::shared_ptr<Render::Texture2D>> m_textures;
	};
}