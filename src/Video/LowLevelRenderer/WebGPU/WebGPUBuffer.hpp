#pragma once

#include "../Interface/Buffer.hpp"

#include "WebGPU.hpp"

namespace Video {

class RawBuffer;

/// WebGPU implementation of Buffer.
class WebGPUBuffer : public Buffer {
  public:
    /// Create new WebGPU buffer.
    /**
     * @param device The WebGPU device.
     * @param bufferUsage How the buffer will be used.
     * @param size The size of the buffer.
     * @param data Data to initialie the buffer with.
     */
    WebGPUBuffer(WGPUDevice device, Buffer::BufferUsage bufferUsage, uint32_t size, const void* data = nullptr);

    /// Destructor.
    ~WebGPUBuffer() final;

    void Reset(BufferUsage bufferUsage, const BufferAllocation& allocation) final;
    unsigned int GetSize() const final;

    /// Get the internal WebGPU buffer.
    /**
     * @param The internal WebGPU buffer.
     */
    WGPUBuffer GetBuffer() const;

  private:
    WebGPUBuffer(const WebGPUBuffer& other) = delete;

    WGPUBuffer buffer;
    uint32_t size = 0;
};

}
