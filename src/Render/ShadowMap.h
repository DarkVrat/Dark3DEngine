#pragma once

#include <glad/glad.h>

namespace Render
{
    class ShadowMap
    {
    public:
        unsigned int depthMapFBO;
        unsigned int depthMap;
        const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;

        ShadowMap();

        void bindDepthMap();
        void unbindFramebuffer();
        void bindShadowMapTexture(int textureUnit);
    };
}
