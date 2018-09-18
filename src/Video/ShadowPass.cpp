#include "ShadowPass.hpp"
#include <glad/glad.h>

namespace Video {
    ShadowPass::ShadowPass() {
        InitDepthMap();
        BindBuffer();
    }

    ShadowPass::~ShadowPass() {
        glDeleteFramebuffers(1, &depthMapFbo);
        glDeleteTextures(1, &depthMap);
    }

    void ShadowPass::InitDepthMap() {
        glGenFramebuffers(1, &depthMapFbo);
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowSize, shadowSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    void ShadowPass::BindBuffer() {
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void ShadowPass::SetShadowMapSize(unsigned int shadowMapSize) {
        shadowSize = shadowMapSize;

        // Recreate GL buffers.
        glDeleteFramebuffers(1, &depthMapFbo);
        glDeleteTextures(1, &depthMap);
        InitDepthMap();
        BindBuffer();
    }

    unsigned int ShadowPass::GetShadowMapSize() const {
        return shadowSize;
    }

    int ShadowPass::GetDepthMapFbo() {
        return depthMapFbo;
    }

    int ShadowPass::GetShadowID() {
        return depthMap;
    }
} // namespace Video
