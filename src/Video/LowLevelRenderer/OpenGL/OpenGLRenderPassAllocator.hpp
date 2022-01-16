#pragma once

#include "../Interface/RenderPassAllocator.hpp"

namespace Video {

/// OpenGL implementation of RenderPassAllocator.
class OpenGLRenderPassAllocator : public RenderPassAllocator {
  public:
    /// Create a new render pass allocator.
    OpenGLRenderPassAllocator();

    /// Destructor.
    ~OpenGLRenderPassAllocator() final;

  private:
    RenderPass* AllocateRenderPass(Texture* colorAttachment, RenderPass::LoadOperation colorLoadOperation, Texture* depthAttachment, RenderPass::LoadOperation depthLoadOperation) final;
    RenderPass* AllocateAttachmentlessRenderPass(const glm::uvec2& size, uint32_t msaaSamples) final;
};

}
