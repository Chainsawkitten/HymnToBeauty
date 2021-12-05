#include "VulkanBufferAllocator.hpp"

#include "VulkanBuffer.hpp"
#include "VulkanRenderer.hpp"
#include "VulkanUtility.hpp"
#include <Utility/Log.hpp>
#include <algorithm>

namespace Video {

static VkBufferUsageFlags GetBufferUsage(Buffer::BufferUsage bufferUsage) {
    VkBufferUsageFlags usage;

    switch (bufferUsage) {
    case Buffer::BufferUsage::VERTEX_BUFFER:
        usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        break;
    case Buffer::BufferUsage::INDEX_BUFFER:
        usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        break;
    case Buffer::BufferUsage::UNIFORM_BUFFER:
        usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        break;
    case Buffer::BufferUsage::STORAGE_BUFFER:
        usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        break;
    case Buffer::BufferUsage::VERTEX_STORAGE_BUFFER:
        usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        break;
    }

    usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    return usage;
}

VulkanRawBuffer::VulkanRawBuffer(VulkanRenderer& vulkanRenderer, VkDevice device, VkPhysicalDevice physicalDevice, Buffer::BufferUsage bufferUsage, bool temporary, unsigned int size) : vulkanRenderer(vulkanRenderer) {
    this->device = device;
    this->physicalDevice = physicalDevice;
    this->temporary = temporary;

    // Create buffer.
    VkBufferUsageFlags usage = GetBufferUsage(bufferUsage);
    VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    if (temporary) {
        memoryPropertyFlags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    }

    Utility::CreateBuffer(device, physicalDevice, size, usage, memoryPropertyFlags, &buffer, &deviceMemory);
}

VulkanRawBuffer::~VulkanRawBuffer() {
    vkDestroyBuffer(device, buffer, nullptr);
    vkFreeMemory(device, deviceMemory, nullptr);
}

void VulkanRawBuffer::Write(uint32_t offset, uint32_t size, const void* data) {
    // Copy data to buffer.
    if (temporary) {
        // Map buffer.
        void* memory;
        vkMapMemory(device, deviceMemory, offset, size, 0, &memory);
        memcpy(memory, data, size);
        vkUnmapMemory(device, deviceMemory);
    } else {
        // Create staging buffer.
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingDeviceMemory;
        Utility::CreateBuffer(device, physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingDeviceMemory);

        // Copy data to staging buffer.
        void* stagingData;
        vkMapMemory(device, stagingDeviceMemory, 0, size, 0, &stagingData);
        memcpy(stagingData, data, size);
        vkUnmapMemory(device, stagingDeviceMemory);

        // Copy from staging buffer to buffer.
        CommandBuffer* commandBuffer = vulkanRenderer.CreateCommandBuffer();
        VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);

        VkBufferCopy region = {};
        region.srcOffset = 0;
        region.dstOffset = offset;
        region.size = size;

        vkCmdCopyBuffer(vulkanCommandBuffer->GetCommandBuffer(), stagingBuffer, buffer, 1, &region);

        vulkanRenderer.Submit(commandBuffer);
        vulkanRenderer.Wait();

        // Cleanup.
        delete commandBuffer;
        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingDeviceMemory, nullptr);
    }
}

VkBuffer VulkanRawBuffer::GetBuffer() const {
    return buffer;
}

VkDeviceMemory VulkanRawBuffer::GetDeviceMemory() const {
    return deviceMemory;
}

VulkanBufferAllocator::VulkanBufferAllocator(VulkanRenderer& vulkanRenderer, VkDevice device, VkPhysicalDevice physicalDevice, uint32_t frames, uint32_t nonCoherentAtomSize) : BufferAllocator(frames), vulkanRenderer(vulkanRenderer) {
    this->device = device;
    this->physicalDevice = physicalDevice;

    for (uint32_t i = 0; i < static_cast<uint32_t>(Buffer::BufferUsage::COUNT); ++i) {
        alignments[i] = CalculateAlignment(static_cast<Buffer::BufferUsage>(i), nonCoherentAtomSize);
    }
}

VulkanBufferAllocator::~VulkanBufferAllocator() {

}

uint32_t VulkanBufferAllocator::GetAlignment(Buffer::BufferUsage bufferUsage) {
    return alignments[static_cast<uint32_t>(bufferUsage)];
}

RawBuffer* VulkanBufferAllocator::Allocate(Buffer::BufferUsage bufferUsage, bool temporary, unsigned int size) {
    return new VulkanRawBuffer(vulkanRenderer, device, physicalDevice, bufferUsage, temporary, size);
}

Buffer* VulkanBufferAllocator::CreateBufferObject(Buffer::BufferUsage bufferUsage, const BufferAllocation& allocation) {
    return new VulkanBuffer(bufferUsage, allocation);
}

uint32_t VulkanBufferAllocator::CalculateAlignment(Buffer::BufferUsage bufferUsage, uint32_t nonCoherentAtomSize) {
    // Create buffer.
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = poolSize;
    bufferCreateInfo.usage = GetBufferUsage(bufferUsage);
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.queueFamilyIndexCount = 0;

    VkBuffer buffer;
    if (vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffer) != VK_SUCCESS) {
        Log(Log::ERR) << "Failed to create buffer.\n";
    }

    // Allocate device memory for the buffer.
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);

    // Cleanup.
    vkDestroyBuffer(device, buffer, nullptr);

    return std::max(static_cast<uint32_t>(memoryRequirements.alignment), nonCoherentAtomSize);
}

}
