#pragma once

#include "../Interface/RenderTargetAllocator.hpp"

namespace Video {

/// OpenGL implementation of RenderTargetAllocator.
class OpenGLRenderTargetAllocator : public RenderTargetAllocator {
  public:
    /// Create a new render target allocator.
    /**
     * @param frames How many frames before re-using buffers.
     */
    explicit OpenGLRenderTargetAllocator(uint8_t frames);

    /// Destructor.
    ~OpenGLRenderTargetAllocator() final;

  private:
    Texture* AllocateRenderTarget(const glm::uvec2& size, Texture::Format format) final;
};

}
