#pragma once

#include "../Interface/BufferAllocator.hpp"
#include "WebGPU.hpp"

namespace Video {

class WebGPURenderer;

/// WebGPU implementation of RawBuffer.
class WebGPURawBuffer : public RawBuffer {
  public:
    /// Create new raw buffer used to satisfy allocations.
    /**
     * @param webGPURenderer The WebGPU renderer.
     * @param bufferUsage How the buffer will be used.
     * @param temporary Whether the buffer is going to be used to satisfy temporary allocations.
     * @param size The size of the buffer in bytes.
     */
    WebGPURawBuffer(WebGPURenderer& webGPURenderer, Buffer::BufferUsage bufferUsage, bool temporary, unsigned int size);

    /// Destructor.
    ~WebGPURawBuffer() final;

    void Write(uint32_t offset, uint32_t size, const void* data) final;

    /// Get the internal WebGPU buffer.
    /**
     * @return The internal WebGPU buffer.
     */
    WGPUBuffer GetBuffer() const;

  private:
    WGPUBuffer buffer;
    WGPUQueue queue;
    bool temporary;
};

/// WebGPU implementation of BufferAllocator.
class WebGPUBufferAllocator : public BufferAllocator {
  public:
    /// Create a new buffer allocator.
    /**
     * @param webGPURenderer The WebGPU renderer.
     * @param frames How many frames before re-using buffers.
     */
    WebGPUBufferAllocator(WebGPURenderer& webGPURenderer, uint32_t frames);

    /// Destructor.
    ~WebGPUBufferAllocator() final;

  private:
    WebGPUBufferAllocator(const WebGPUBufferAllocator& other) = delete;

    uint32_t GetAlignment(Buffer::BufferUsage bufferUsage) final;
    RawBuffer* Allocate(Buffer::BufferUsage bufferUsage, bool temporary, unsigned int size) final;
    Buffer* CreateBufferObject(Buffer::BufferUsage bufferUsage, const BufferAllocation& allocation) final;

    WebGPURenderer& webGPURenderer;
};

}
