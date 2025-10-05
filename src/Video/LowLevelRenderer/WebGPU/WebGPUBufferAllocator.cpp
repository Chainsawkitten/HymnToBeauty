#include "WebGPUBufferAllocator.hpp"

#include "WebGPUBuffer.hpp"
#include "WebGPURenderer.hpp"

namespace Video {

static WGPUBufferUsageFlags GetBufferUsage(Buffer::BufferUsage bufferUsage) {
    WGPUBufferUsageFlags usage;

    switch (bufferUsage) {
    case Buffer::BufferUsage::VERTEX_BUFFER:
        usage = WGPUBufferUsage_Vertex;
        break;
    case Buffer::BufferUsage::INDEX_BUFFER:
        usage = WGPUBufferUsage_Index;
        break;
    case Buffer::BufferUsage::UNIFORM_BUFFER:
        usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst;
        break;
    case Buffer::BufferUsage::STORAGE_BUFFER:
        usage = WGPUBufferUsage_Storage | WGPUBufferUsage_CopyDst;
        break;
    case Buffer::BufferUsage::VERTEX_STORAGE_BUFFER:
        usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_Storage | WGPUBufferUsage_CopyDst;
        break;
    }

    // For wgpuQueueWriteBuffer.
    usage |= WGPUBufferUsage_CopyDst;

    return usage;
}

WebGPURawBuffer::WebGPURawBuffer(WebGPURenderer& webGPURenderer, Buffer::BufferUsage bufferUsage, bool temporary, unsigned int size) {
    this->temporary = temporary;
    queue = webGPURenderer.GetQueue();

    // Create buffer.
    WGPUBufferDescriptor bufferDescriptor = {};
    bufferDescriptor.usage = GetBufferUsage(bufferUsage);
    bufferDescriptor.size = size;

    buffer = wgpuDeviceCreateBuffer(webGPURenderer.GetDevice(), &bufferDescriptor);
}

WebGPURawBuffer::~WebGPURawBuffer() {
    wgpuBufferRelease(buffer);
}

void WebGPURawBuffer::Write(uint32_t offset, uint32_t size, const void* data) {
    /// @todo Could use mapOnCreate for non-temporary buffers?
    wgpuQueueWriteBuffer(queue, buffer, offset, data, size);
}

WGPUBuffer WebGPURawBuffer::GetBuffer() const {
    return buffer;
}

WebGPUBufferAllocator::WebGPUBufferAllocator(WebGPURenderer& webGPURenderer, uint32_t frames) : BufferAllocator(frames), webGPURenderer(webGPURenderer) {

}

WebGPUBufferAllocator::~WebGPUBufferAllocator() {

}

uint32_t WebGPUBufferAllocator::GetAlignment(Buffer::BufferUsage bufferUsage) {
    // Both minUniformBufferOffsetAlignment and minStorageBufferOffsetAlignment are guaranteed to be at most 256.
    // https://www.w3.org/TR/webgpu/#dom-gpusupportedlimits-minuniformbufferoffsetalignment
    return 256;
}

RawBuffer* WebGPUBufferAllocator::Allocate(Buffer::BufferUsage bufferUsage, bool temporary, unsigned int size) {
    // Buffer binding sizes must be a multiple of 16.
    size = (size + 16 - 1) / 16 * 16;

    return new WebGPURawBuffer(webGPURenderer, bufferUsage, temporary, size);
}

Buffer* WebGPUBufferAllocator::CreateBufferObject(Buffer::BufferUsage bufferUsage, const BufferAllocation& allocation) {
    BufferAllocation realAllocation = allocation;
    if (bufferUsage == Buffer::BufferUsage::UNIFORM_BUFFER || bufferUsage == Buffer::BufferUsage::STORAGE_BUFFER || bufferUsage == Buffer::BufferUsage::VERTEX_STORAGE_BUFFER) {
        // Buffer binding sizes must be a multiple of 16.
        realAllocation.size = (allocation.size + 16 - 1) / 16 * 16;
    }

    return new WebGPUBuffer(bufferUsage, realAllocation);
}

}
