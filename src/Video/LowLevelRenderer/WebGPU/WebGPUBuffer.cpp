#include "WebGPUBuffer.hpp"

#include "WebGPUBufferAllocator.hpp"
#include <Utility/Log.hpp>
#include <cstring>

namespace Video {

WebGPUBuffer::WebGPUBuffer(Buffer::BufferUsage bufferUsage, const BufferAllocation& allocation) : Buffer(bufferUsage) {
    Reset(bufferUsage, allocation);
}

WebGPUBuffer::~WebGPUBuffer() {
    if (!temporaryAllocation) {
        delete rawBuffer;
    }
}

void WebGPUBuffer::Reset(BufferUsage bufferUsage, const BufferAllocation& allocation) {
    this->bufferUsage = bufferUsage;
    rawBuffer = allocation.buffer;
    WebGPURawBuffer* webGPURawBuffer = static_cast<WebGPURawBuffer*>(rawBuffer);
    buffer = webGPURawBuffer->GetBuffer();
    offset = allocation.offset;
    size = allocation.size;
    temporaryAllocation = allocation.temporaryAllocation;
}

unsigned int WebGPUBuffer::GetSize() const {
    return size;
}

WGPUBuffer WebGPUBuffer::GetBuffer() const {
    return buffer;
}

uint32_t WebGPUBuffer::GetOffset() const {
    return offset;
}

} // namespace Video
