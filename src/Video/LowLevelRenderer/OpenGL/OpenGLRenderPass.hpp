#pragma once

#include "../Interface/RenderPass.hpp"

#include <glad/gl.h>

namespace Video {

class Texture;

/// OpenGL implementation of RenderPass.
class OpenGLRenderPass : public RenderPass {
  public:
    /// Create new OpenGL render pass.
    /**
     * @param colorAttachment The color attachment to render to.
     * @param colorLoadOperation What to do with the previous contents of the color attachment.
     * @param depthAttachment The depth attachment to render to.
     * @param depthLoadOperation What to do with the previous contents of the depth attachment.
     */
    OpenGLRenderPass(Texture* colorAttachment, RenderPass::LoadOperation colorLoadOperation, Texture* depthAttachment, RenderPass::LoadOperation depthLoadOperation);

    /// Create new attachmentless OpenGL render pass.
    /**
     * @param size The size of the framebuffer.
     * @param msaaSamples The number of MSAA samples.
     */
    OpenGLRenderPass(const glm::uvec2& size, uint32_t msaaSamples);

    /// Destructor.
    ~OpenGLRenderPass() final;

    const glm::uvec2& GetSize() const final;

    /// Get the render pass' frame buffer.
    /**
     * @return The frame buffer.
     */
    GLuint GetFrameBuffer() const;

    /// Get the clear mask for use when beginning the render pass.
    /**
     * @return The clear mask.
     */
    GLbitfield GetClearMask() const;

  private:
    OpenGLRenderPass(const OpenGLRenderPass& other) = delete;

    GLuint frameBuffer;
    glm::uvec2 size;

    GLbitfield clearMask;
};

}
