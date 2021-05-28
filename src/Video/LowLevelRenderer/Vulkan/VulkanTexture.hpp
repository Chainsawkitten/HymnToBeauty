#pragma once

#include "../Interface/Texture.hpp"

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace Video {

class VulkanRenderer;

/// Vulkan implementation of Texture.
class VulkanTexture : public Texture {
  public:
    /// Create new Vulkan texture.
    /**
     * @param vulkanRenderer The Vulkan renderer.
     * @param device The Vulkan device.
     * @param physicalDevice The physical device.
     * @param size The size of the texture, in pixels.
     * @param type The type of texture to create.
     * @param format The format of the texture.
     * @param components The number of components in the texture, 0 if no texture data is supplied.
     * @param data The texture data to upload, or nullptr.
     */
    VulkanTexture(VulkanRenderer& vulkanRenderer, VkDevice device, VkPhysicalDevice physicalDevice, const glm::uvec2 size, Texture::Type type, Texture::Format format, int components = 0, unsigned char* data = nullptr);

    /// Destructor.
    ~VulkanTexture() final;

    /// Get the format of the texture.
    /**
     * @return The format of the texture.
     */
    VkFormat GetFormat() const;

    /// Get the image.
    /**
     * @return The image.
     */
    VkImage GetImage() const;

    /// Get image view.
    /**
     * @return The image view.
     */
    VkImageView GetImageView() const;

    /// Get the sampler.
    /**
     * @return The sampler.
     */
    VkSampler GetSampler() const;

    /// Get the current image layout.
    /**
     * @return The image layout.
     */
    VkImageLayout GetImageLayout() const;

    /// Set the current image layout.
    /**
     * @param layout The new image layout.
     */
    void SetImageLayout(VkImageLayout layout);

  private:
    VulkanTexture(const VulkanTexture& other) = delete;

    void GenerateMipMaps(VulkanRenderer& vulkanRenderer, VkPhysicalDevice physicalDevice, const glm::uvec2& size, uint32_t mipLevels, VkFormat format);

    VkDevice device;

    VkFormat internalFormat;
    VkImage image;
    VkDeviceMemory deviceMemory;
    VkImageView imageView;
    VkSampler sampler = VK_NULL_HANDLE;
    VkImageLayout currentLayout;
};

}
