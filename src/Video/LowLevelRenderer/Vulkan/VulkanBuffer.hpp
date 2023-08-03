#pragma once

#include "../Interface/Buffer.hpp"

#include <glad/vulkan.h>

namespace Video {

class RawBuffer;
class VulkanRenderer;

/// Vulkan implementation of Buffer.
class VulkanBuffer : public Buffer {
  public:
    /// Create new Vulkan buffer.
    /**
     * @param bufferUsage How the buffer will be used.
     * @param allocation The allocation to encapsulate.
     */
    VulkanBuffer(Buffer::BufferUsage bufferUsage, const BufferAllocation& allocation);

    /// Destructor.
    ~VulkanBuffer() final;

    void Reset(BufferUsage bufferUsage, const BufferAllocation& allocation) final;
    unsigned int GetSize() const final;

    /// Get the internal Vulkan buffer.
    /**
     * @return The internal Vulkan buffer.
     */
    VkBuffer GetBuffer() const;

    /// Get the offset into the raw buffer.
    /**
     * @return The offset into the raw buffer.
     */
    uint32_t GetOffset() const;

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
    VkBuffer buffer;
    VkDeviceMemory deviceMemory;
    uint32_t offset;
    uint32_t size;

    RawBuffer* rawBuffer;
    bool temporaryAllocation;

    VkPipelineStageFlags readMask;
    VkPipelineStageFlags lastWrite;
};

}
