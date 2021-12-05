#include "VulkanBuffer.hpp"

#include <Utility/Log.hpp>
#include "VulkanBufferAllocator.hpp"
#include "VulkanRenderer.hpp"

namespace Video {

VulkanBuffer::VulkanBuffer(Buffer::BufferUsage bufferUsage, const BufferAllocation& allocation) : Buffer(bufferUsage) {
    Reset(bufferUsage, allocation);
}

VulkanBuffer::~VulkanBuffer() {
    if (!temporaryAllocation) {
        delete rawBuffer;
    }
}

void VulkanBuffer::Reset(BufferUsage bufferUsage, const BufferAllocation& allocation) {
    this->bufferUsage = bufferUsage;
    rawBuffer = allocation.buffer;
    VulkanRawBuffer* vulkanRawBuffer = static_cast<VulkanRawBuffer*>(rawBuffer);
    buffer = vulkanRawBuffer->GetBuffer();
    deviceMemory = vulkanRawBuffer->GetDeviceMemory();
    offset = allocation.offset;
    size = allocation.size;
    temporaryAllocation = allocation.temporaryAllocation;

    readMask = 0;
    lastWrite = 0;
}

unsigned int VulkanBuffer::GetSize() const {
    return size;
}

VkBuffer VulkanBuffer::GetBuffer() const {
    return buffer;
}

uint32_t VulkanBuffer::GetOffset() const {
    return offset;
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
