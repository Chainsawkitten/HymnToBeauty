#include "OpenGLRenderPass.hpp"

#include <assert.h>
#include <Utility/Log.hpp>
#include "OpenGLTexture.hpp"

namespace Video {

OpenGLRenderPass::OpenGLRenderPass(Texture* colorAttachment, RenderPass::LoadOperation colorLoadOperation, Texture* depthAttachment, RenderPass::LoadOperation depthLoadOperation) {
    assert(colorAttachment != nullptr || depthAttachment != nullptr);

    // Frame buffer object.
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    if (colorAttachment != nullptr) {
        assert(colorAttachment->GetType() == Texture::Type::RENDER_COLOR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, static_cast<OpenGLTexture*>(colorAttachment)->GetID(), 0);

        // Specify which buffers are drawn into.
        GLenum drawBuffer = GL_COLOR_ATTACHMENT0;
        glDrawBuffers(1, &drawBuffer);

        size = colorAttachment->GetSize();
    }
    
    if (depthAttachment != nullptr) {
        assert(depthAttachment->GetType() == Texture::Type::RENDER_DEPTH);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, static_cast<OpenGLTexture*>(depthAttachment)->GetID(), 0);

        size = depthAttachment->GetSize();
    }

    // Check if framebuffer was created correctly
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Log(Log::ERR) << "Framebuffer creation failed\n";
    }

    // Default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Set the clear mask based on the load operations.
    clearMask = 0;
    if (colorAttachment != nullptr && colorLoadOperation != RenderPass::LoadOperation::LOAD)
        clearMask |= GL_COLOR_BUFFER_BIT;

    if (depthAttachment != nullptr && depthLoadOperation != RenderPass::LoadOperation::LOAD)
        clearMask |= GL_DEPTH_BUFFER_BIT;
}

OpenGLRenderPass::OpenGLRenderPass(const glm::uvec2& size, uint32_t msaaSamples) {
    assert(size.x > 0 && size.y > 0);

    // Frame buffer object.
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, size.x);
    glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, size.y);
    glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_SAMPLES, msaaSamples);

    this->size = size;

    // Check if framebuffer was created correctly
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Log(Log::ERR) << "Framebuffer creation failed\n";
    }

    // Default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    clearMask = 0;
}

OpenGLRenderPass::~OpenGLRenderPass() {
    glDeleteFramebuffers(1, &frameBuffer);
}

const glm::uvec2& OpenGLRenderPass::GetSize() const {
    return size;
}

GLuint OpenGLRenderPass::GetFrameBuffer() const {
    return frameBuffer;
}

GLbitfield OpenGLRenderPass::GetClearMask() const {
    return clearMask;
}

}
