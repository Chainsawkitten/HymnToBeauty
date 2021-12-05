#pragma once

#include "../Interface/BufferAllocator.hpp"
#include <vulkan/vulkan.h>

namespace Video {

class VulkanRenderer;

/// Vulkan implementation of RawBuffer.
class VulkanRawBuffer : public RawBuffer {
  public:
    /// Create new raw buffer used to satisfy allocations.
    /**
     * @param vulkanRenderer The Vulkan renderer.
     * @param device The Vulkan device.
     * @param physicalDevice The physical device.
     * @param bufferUsage How the buffer will be used.
     * @param temporary Whether the buffer is going to be used to satisfy temporary allocations.
     * @param size The size of the buffer in bytes.
     */
    VulkanRawBuffer(VulkanRenderer& vulkanRenderer, VkDevice device, VkPhysicalDevice physicalDevice, Buffer::BufferUsage bufferUsage, bool temporary, unsigned int size);

    /// Destructor.
    ~VulkanRawBuffer() final;

    void Write(uint32_t offset, uint32_t size, const void* data) final;

    /// Get the internal Vulkan buffer.
    /**
     * @return The internal Vulkan buffer.
     */
    VkBuffer GetBuffer() const;

    /// Get the device memory.
    /**
     * @return The device memory.
     */
    VkDeviceMemory GetDeviceMemory() const;

  private:
    VulkanRenderer& vulkanRenderer;
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkBuffer buffer;
    VkDeviceMemory deviceMemory;
    bool temporary;
};

/// Vulkan implementation of BufferAllocator.
class VulkanBufferAllocator : public BufferAllocator {
  public:
    /// Create a new buffer allocator.
    /**
     * @param vulkanRenderer The Vulkan renderer.
     * @param device The Vulkan device.
     * @param physicalDevice The physical device.
     * @param frames How many frames before re-using buffers.
     * @param nonCoherentAtomSize The size and alignment in bytes that bounds concurrent access to host-mapped device memory.
     */
    VulkanBufferAllocator(VulkanRenderer& vulkanRenderer, VkDevice device, VkPhysicalDevice physicalDevice, uint32_t frames, uint32_t nonCoherentAtomSize);

    /// Destructor.
    ~VulkanBufferAllocator() final;

  private:
    VulkanBufferAllocator(const VulkanBufferAllocator& other) = delete;

    uint32_t GetAlignment(Buffer::BufferUsage bufferUsage) final;
    RawBuffer* Allocate(Buffer::BufferUsage bufferUsage, bool temporary, unsigned int size) final;
    Buffer* CreateBufferObject(Buffer::BufferUsage bufferUsage, const BufferAllocation& allocation) final;

    uint32_t CalculateAlignment(Buffer::BufferUsage bufferUsage, uint32_t nonCoherentAtomSize);

    VulkanRenderer& vulkanRenderer;
    VkDevice device;
    VkPhysicalDevice physicalDevice;

    uint32_t alignments[static_cast<uint32_t>(Buffer::BufferUsage::COUNT)];
};

}
