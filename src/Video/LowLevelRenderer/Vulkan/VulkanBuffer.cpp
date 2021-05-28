#include "VulkanBuffer.hpp"

#include <Utility/Log.hpp>
#include "VulkanCommandBuffer.hpp"
#include "VulkanRenderer.hpp"
#include "VulkanUtility.hpp"
#include <cstring>

namespace Video {

VulkanBuffer::VulkanBuffer(VulkanRenderer& vulkanRenderer, VkDevice device, VkPhysicalDevice physicalDevice, Buffer::BufferUsage bufferUsage, unsigned int size, const void* data) : Buffer(bufferUsage) {
    this->device = device;
    this->size = size;

    swapChainImages = vulkanRenderer.GetSwapChainImageCount();

    // Create buffer.
    VkBufferUsageFlags usage;
    VkMemoryPropertyFlags memoryPropertyFlags;

    switch (bufferUsage) {
    case BufferUsage::VERTEX_BUFFER_STATIC:
        usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        swapChainImages = 1;
        break;
    case BufferUsage::VERTEX_BUFFER_DYNAMIC:
        usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        break;
    case BufferUsage::INDEX_BUFFER_STATIC:
        usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        swapChainImages = 1;
        break;
    case BufferUsage::INDEX_BUFFER_DYNAMIC:
        usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        break;
    case BufferUsage::UNIFORM_BUFFER:
        usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        break;
    case BufferUsage::STORAGE_BUFFER:
        usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        break;
    case BufferUsage::VERTEX_STORAGE_BUFFER:
        usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        break;
    }

    usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    buffer = new VkBuffer[swapChainImages];
    deviceMemory = new VkDeviceMemory[swapChainImages];
    for (uint32_t i = 0; i < swapChainImages; ++i) {
        Utility::CreateBuffer(device, physicalDevice, size, usage, memoryPropertyFlags, &buffer[i], &deviceMemory[i]);
    }

    // Copy data to buffer.
    if (data != nullptr) {
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
        region.dstOffset = 0;
        region.size = size;

        vkCmdCopyBuffer(vulkanCommandBuffer->GetCommandBuffer(), stagingBuffer, buffer[0], 1, &region);

        vulkanRenderer.Submit(commandBuffer);
        vulkanRenderer.Wait();

        // Cleanup.
        delete commandBuffer;
        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingDeviceMemory, nullptr);
    }
}

VulkanBuffer::~VulkanBuffer() {
    for (uint32_t i = 0; i < swapChainImages; ++i) {
        vkDestroyBuffer(device, buffer[i], nullptr);
        vkFreeMemory(device, deviceMemory[i], nullptr);
    }
}

void VulkanBuffer::Write(const void* data) {
    assert(GetBufferUsage() == BufferUsage::VERTEX_BUFFER_DYNAMIC || GetBufferUsage() == BufferUsage::INDEX_BUFFER_DYNAMIC || GetBufferUsage() == BufferUsage::UNIFORM_BUFFER || GetBufferUsage() == BufferUsage::STORAGE_BUFFER || GetBufferUsage() == BufferUsage::VERTEX_STORAGE_BUFFER);

    currentFrame = (currentFrame + 1) % swapChainImages;

    void* memory;
    vkMapMemory(device, deviceMemory[currentFrame], 0, size, 0, &memory);
    memcpy(memory, data, size);
    vkUnmapMemory(device, deviceMemory[currentFrame]);
}

unsigned int VulkanBuffer::GetSize() const {
    return size;
}

VkBuffer VulkanBuffer::GetBuffer() const {
    return buffer[currentFrame];
}

VkPipelineStageFlags VulkanBuffer::GetReadMask() const {
    return readMask;
}

void VulkanBuffer::SetReadMaskStage(VkPipelineStageFlags pipelineStage) {
    readMask |= pipelineStage;
}

void VulkanBuffer::ClearReadMask() {
    readMask = 0;
}

VkPipelineStageFlags VulkanBuffer::GetLastWriteStage() const {
    return lastWrite;
}

void VulkanBuffer::SetLastWriteStage(VkPipelineStageFlags pipelineStage) {
    lastWrite = pipelineStage;
}

} // namespace Video
