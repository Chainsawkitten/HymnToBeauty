#pragma once

#include "../Interface/RenderPassAllocator.hpp"
#include <glad/vulkan.h>

namespace Video {

/// Vulkan implementation of RenderPassAllocator.
class VulkanRenderPassAllocator : public RenderPassAllocator {
  public:
    /// Create a new render pass allocator.
    /**
     * @param device The Vulkan device.
     */
    explicit VulkanRenderPassAllocator(VkDevice device);

    /// Destructor.
    ~VulkanRenderPassAllocator() final;

  private:
    RenderPass* AllocateRenderPass(Texture* colorAttachment, RenderPass::LoadOperation colorLoadOperation, Texture* depthAttachment, RenderPass::LoadOperation depthLoadOperation) final;
    RenderPass* AllocateAttachmentlessRenderPass(const glm::uvec2& size, uint32_t msaaSamples) final;

    VkDevice device;
};

}
