#pragma once

#include "../Interface/Buffer.hpp"

#include <vulkan/vulkan.h>

namespace Video {

class VulkanRenderer;

/// Vulkan implementation of Buffer.
class VulkanBuffer : public Buffer {
  public:
    /// Create new Vulkan buffer.
    /**
     * @param vulkanRenderer The Vulkan renderer.
     * @param device The Vulkan device.
     * @param physicalDevice The physical device.
     * @param bufferUsage How the buffer will be used.
     * @param size The size of the buffer in bytes.
     * @param data Data to upload to the buffer. Can be nullptr if no data should be uploaded.
     */
    VulkanBuffer(VulkanRenderer& vulkanRenderer, VkDevice device, VkPhysicalDevice physicalDevice, Buffer::BufferUsage bufferUsage, unsigned int size, const void* data = nullptr);

    /// Destructor.
    ~VulkanBuffer() final;

    void Write(const void* data) final;
    unsigned int GetSize() const final;

    /// Get the internal Vulkan buffer.
    /**
     * @return The internal Vulkan buffer.
     */
    VkBuffer GetBuffer() const;

  private:
    VulkanBuffer(const VulkanBuffer& other) = delete;

    VkDevice device;
    VkBuffer* buffer;
    VkDeviceMemory* deviceMemory;
    uint32_t size;

    uint32_t currentFrame = 0;
    uint32_t swapChainImages;
};

}
