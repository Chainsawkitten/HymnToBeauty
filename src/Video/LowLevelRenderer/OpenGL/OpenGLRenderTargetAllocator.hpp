#pragma once

#include "../Interface/RenderTargetAllocator.hpp"

namespace Video {

/// OpenGL implementation of RenderTargetAllocator.
class OpenGLRenderTargetAllocator : public RenderTargetAllocator {
  public:
    /// Create a new render target allocator.
    /**
     * @param frames How many frames before re-using buffers.
     * @param renderPassAllocator Render pass allocator.
     */
    OpenGLRenderTargetAllocator(uint8_t frames, RenderPassAllocator* renderPassAllocator);

    /// Destructor.
    ~OpenGLRenderTargetAllocator() final;

  private:
    Texture* AllocateRenderTarget(const glm::uvec2& size, Texture::Format format) final;
};

}
