#pragma once

#include "../Interface/RenderPass.hpp"

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace Video {

class Texture;
class VulkanTexture;

/// Vulkan implementation of RenderPass.
class VulkanRenderPass : public RenderPass {
  public:
    /// Create new Vulkan render pass.
    /**
     * @param device The Vulkan device.
     * @param colorAttachment The color attachment to draw to.
     * @param colorLoadOperation What to do with the previous contents of the color attachment.
     * @param depthAttachment The depth attachment to draw to.
     * @param depthLoadOperation What to do with the previous contents of the depth attachment.
     */
    VulkanRenderPass(VkDevice device, Texture* colorAttachment, RenderPass::LoadOperation colorLoadOperation, Texture* depthAttachment, RenderPass::LoadOperation depthLoadOperation);

    /// Destructor.
    ~VulkanRenderPass() final;

    const glm::uvec2& GetSize() const final;

    /// Get the Vulkan render pass.
    /**
     * @return The internal Vulkan render pass.
     */
    VkRenderPass GetRenderPass() const;

    /// Get the Vulkan framebuffer.
    /**
     * @return The internal Vulkan framebuffer.
     */
    VkFramebuffer GetFramebuffer() const;

    /// Get whether there is a color attachment.
    /**
     * @return Whether there is a color attachment.
     */
    bool HasColorAttachment() const;

    /// Get the first color attachment.
    /**
     * @return The first color attachment.
     */
    VulkanTexture* GetColorAttachment() const;

    /// Get whether there is a depth attachment.
    /**
     * @return Whether there is a depth attachment.
     */
    bool HasDepthAttachment() const;

    /// Get the depth attachment.
    /**
     * @return The depth attachment.
     */
    VulkanTexture* GetDepthAttachment() const;

  private:
    VulkanRenderPass(const VulkanRenderPass& other) = delete;

    VkDevice device;
    VkRenderPass renderPass;
    VkFramebuffer framebuffer;
    VkImage colorImage;
    bool hasColorAttachment;
    VulkanTexture* colorAttachment;
    bool hasDepthAttachment;
    VulkanTexture* depthAttachment;

    glm::uvec2 size;
};

} // namespace Video
