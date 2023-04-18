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
     * @param bufferUsage How the buffer will be used.
     * @param allocation The allocation to encapsulate.
     */
    WebGPUBuffer(Buffer::BufferUsage bufferUsage, const BufferAllocation& allocation);

    /// Destructor.
    ~WebGPUBuffer() final;

    void Reset(BufferUsage bufferUsage, const BufferAllocation& allocation) final;
    unsigned int GetSize() const final;

    /// Get the internal WebGPU buffer.
    /**
     * @param The internal WebGPU buffer.
     */
    WGPUBuffer GetBuffer() const;

    /// Get the offset into the raw buffer.
    /**
     * @return The offset into the raw buffer.
     */
    uint32_t GetOffset() const;

  private:
    WebGPUBuffer(const WebGPUBuffer& other) = delete;

    RawBuffer* rawBuffer;
    WGPUBuffer buffer;
    uint32_t offset = 0;
    uint32_t size = 0;
    bool temporaryAllocation;
};

}
