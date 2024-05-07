#include "TextureLoader.h"

std::map<std::string, std::shared_ptr<Render::Texture2D>> Managers::TextureLoader::m_textures;

namespace Managers
{
    std::shared_ptr<Render::Texture2D> TextureLoader::loadTexture(const std::string& texturePath)
    {
        auto texture = m_textures.find(texturePath);
        if (texture != m_textures.end())
            return texture->second;

        std::shared_ptr<Render::Texture2D> texture_ptr = std::make_shared<Render::Texture2D>(texturePath);
        m_textures.emplace(texturePath, texture_ptr);
        return texture_ptr;
    }
}