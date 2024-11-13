#pragma once

#include <glad/glad.h>

namespace Render
{
    class ShadowPoint
    {
    public:
        unsigned int depthMapFBO;
        unsigned int depthCubemap;
        const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

        ShadowPoint();

        void bindDepthMap();
        void unbindFramebuffer();
        void bindShadowMapTexture(int textureUnit);
    };
}
