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

    /// Get which pipeline stages have read the buffer since the last write.
    /**
     * @return A mask of all pipeline stages which have read the buffer.
     */
    VkPipelineStageFlags GetReadMask() const;

    /// Set a pipeline stage having read the buffer since the last write.
    /**
     * @param pipelineStage The pipeline stage reading the buffer.
     */
    void SetReadMaskStage(VkPipelineStageFlags pipelineStage);

    /// Clear the read mask.
    void ClearReadMask();

    /// Get the shader stage which last wrote to the buffer.
    /**
     * @return The last shader stage which wrote to the buffer (or 0 if none).
     */
    VkPipelineStageFlags GetLastWriteStage() const;

    /// Set which pipeline stage last wrote to the buffer.
    /**
     * @param pipelineStage The pipeline stage writing to the buffer.
     */
    void SetLastWriteStage(VkPipelineStageFlags pipelineStage);

  private:
    VulkanBuffer(const VulkanBuffer& other) = delete;

    VkDevice device;
    VkBuffer* buffer;
    VkDeviceMemory* deviceMemory;
    uint32_t size;

    uint32_t currentFrame = 0;
    uint32_t swapChainImages;

    VkPipelineStageFlags readMask = 0;
    VkPipelineStageFlags lastWrite = 0;
};

}
