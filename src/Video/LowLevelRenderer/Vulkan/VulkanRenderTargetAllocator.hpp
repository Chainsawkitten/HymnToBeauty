#pragma once

#include "../Interface/RenderTargetAllocator.hpp"

#include <vulkan/vulkan.h>

namespace Video {

class VulkanRenderer;

/// Vulkan implementation of RenderTargetAllocator.
class VulkanRenderTargetAllocator : public RenderTargetAllocator {
  public:
    /// Create a new render target allocator.
    /**
     * @param vulkanRenderer The Vulkan renderer.
     * @param device The Vulkan device.
     * @param physicalDevice The physical device.
     * @param renderPassAllocator Render pass allocator.
     */
    VulkanRenderTargetAllocator(VulkanRenderer& vulkanRenderer, VkDevice device, VkPhysicalDevice physicalDevice, RenderPassAllocator* renderPassAllocator);

    /// Destructor.
    ~VulkanRenderTargetAllocator() final;

  private:
    Texture* AllocateRenderTarget(const glm::uvec2& size, Texture::Format format) final;

    VulkanRenderer& vulkanRenderer;
    VkDevice device;
    VkPhysicalDevice physicalDevice;
};

}
