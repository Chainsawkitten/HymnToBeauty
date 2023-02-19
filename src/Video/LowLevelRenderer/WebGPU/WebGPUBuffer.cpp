#include "WebGPUBuffer.hpp"

#include <Utility/Log.hpp>
#include <cstring>

namespace Video {

WebGPUBuffer::WebGPUBuffer(WGPUDevice device, Buffer::BufferUsage bufferUsage, uint32_t size, const void* data) : Buffer(bufferUsage) {
    WGPUBufferDescriptor bufferDescriptor = {};
    switch (bufferUsage) {
    case Buffer::BufferUsage::VERTEX_BUFFER:
        bufferDescriptor.usage = WGPUBufferUsage_Vertex;
        break;
    case Buffer::BufferUsage::INDEX_BUFFER:
        bufferDescriptor.usage = WGPUBufferUsage_Index;
        break;
    case Buffer::BufferUsage::UNIFORM_BUFFER:
        bufferDescriptor.usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst;
        // Buffer binding sizes must be a multiple of 16.
        size = (size + 16 - 1) / 16 * 16;
        break;
    case Buffer::BufferUsage::STORAGE_BUFFER:
        bufferDescriptor.usage = WGPUBufferUsage_Storage | WGPUBufferUsage_CopyDst;
        // Buffer binding sizes must be a multiple of 16.
        size = (size + 16 - 1) / 16 * 16;
        break;
    case Buffer::BufferUsage::VERTEX_STORAGE_BUFFER:
        bufferDescriptor.usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_Storage | WGPUBufferUsage_CopyDst;
        // Buffer binding sizes must be a multiple of 16.
        size = (size + 16 - 1) / 16 * 16;
        break;
    }

    if (data != nullptr) {
        // We are allowed to set mappedAtCreation even without MAP_READ or MAP_WRITE usage flags. This allows us to initialize the buffer with data.
        bufferDescriptor.mappedAtCreation = true;

        // Buffers that are mapped at creation must have a size that is a multiple of 4.
        size = (size + 4 - 1) / 4 * 4;
    }

    this->size = size;
    bufferDescriptor.size = size;

    buffer = wgpuDeviceCreateBuffer(device, &bufferDescriptor);

    if (data != nullptr) {
        // Copy the initial data.
        void* mappedData = wgpuBufferGetMappedRange(buffer, 0, size);
        memcpy(mappedData, data, size);
        wgpuBufferUnmap(buffer);
    }
}

WebGPUBuffer::~WebGPUBuffer() {
    wgpuBufferRelease(buffer);
}

void WebGPUBuffer::Reset(BufferUsage bufferUsage, const BufferAllocation& allocation) {
    assert(false);

    Log(Log::ERR) << "WebGPUBuffer::Reset should never be called.\n";
}

unsigned int WebGPUBuffer::GetSize() const {
    return size;
}

WGPUBuffer WebGPUBuffer::GetBuffer() const {
    return buffer;
}

} // namespace Video
