#include "Texture2D.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Render 
{
    Texture2D::Texture2D(const std::string& TexturePath)
    {
        glGenTextures(1, &m_id);
        glBindTexture(GL_TEXTURE_2D, m_id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_set_flip_vertically_on_load(true);
#ifdef NDEBUG
        unsigned char* data = stbi_load(TexturePath.c_str(), &m_width, &m_height, &m_channels, 0);
#else
        std::string path = "C:/Users/DarkVrat/Desktop/Dark3DEngine/" + TexturePath;
        unsigned char* data = stbi_load(path.c_str(), &m_width, &m_height, &m_channels, 0);
#endif

        if (data)
        {
            if(m_channels==3)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            else
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
            std::cout << "Failed to load texture" << std::endl;

        stbi_image_free(data);
    }

    Texture2D::Texture2D(Texture2D&& OtherTexture) noexcept
    {
        m_id = OtherTexture.m_id;
        m_activeId = OtherTexture.m_activeId;
        m_width = OtherTexture.m_width;
        m_height = OtherTexture.m_height;
        m_channels = OtherTexture.m_channels;

        OtherTexture.m_id = 0;
        OtherTexture.m_activeId = 0;
        OtherTexture.m_width = 0;
        OtherTexture.m_height = 0;
        OtherTexture.m_channels = 0;
    }

    Texture2D& Texture2D::operator=(Texture2D&& OtherTexture) noexcept
    {
        if (m_id == OtherTexture.m_id) return *this;

        m_id = OtherTexture.m_id;
        m_activeId = OtherTexture.m_activeId;
        m_width = OtherTexture.m_width;
        m_height = OtherTexture.m_height;
        m_channels = OtherTexture.m_channels;

        OtherTexture.m_id = 0;
        OtherTexture.m_activeId = 0;
        OtherTexture.m_width = 0;
        OtherTexture.m_height = 0;
        OtherTexture.m_channels = 0;

        return *this;
    }

    Texture2D::~Texture2D()
    {
        glDeleteTextures(1, &m_id);
    }

    void Texture2D::setParam(GLuint WrapS, GLuint WrapT, GLuint MinFilter, GLuint MagFilter) const noexcept
    {
        bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MagFilter);
    }

    void Texture2D::setActiveId(GLuint id) noexcept
    {
        m_activeId = id;
    }

    void Texture2D::bind() const noexcept
    {
        glActiveTexture(GL_TEXTURE0 + m_activeId);
        glBindTexture(GL_TEXTURE_2D, m_id);
    }
}
