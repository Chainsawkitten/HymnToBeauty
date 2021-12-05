#pragma once

#include <cstdint>
#include <vector>
#include "Buffer.hpp"

namespace Video {

/// A large buffer from which buffers are sub-allocated.
class RawBuffer {
    public:
    /// Destructor.
    virtual ~RawBuffer() {};

    /// Write data to buffer.
    /**
     * @param offset The offset (in bytes) into the buffer to write at.
     * @param size The size of the data to write (in bytes).
     * @param data The data to write.
     */
    virtual void Write(uint32_t offset, uint32_t size, const void* data) = 0;
};

/// An allocation.
struct BufferAllocation {
    /// The buffer the allocation was made in.
    RawBuffer* buffer;

    /// The offset into the buffer.
    uint32_t offset;

    /// The size of the allocation.
    uint32_t size;

    /// Whether the allocation was temporary (buffer is not responsible for freeing memory).
    bool temporaryAllocation;
};

/// Responsible for allocating buffers.
class BufferAllocator {
  public:
    /// Create a new buffer allocator.
    /**
     * @param frames How many frames before re-using buffers.
     */
    explicit BufferAllocator(uint32_t frames);

    /// Destructor.
    virtual ~BufferAllocator();

    /// Call at the beginning of each frame.
    void BeginFrame();

    /// Create a GPU buffer.
    /**
     * @param bufferUsage How the buffer will be used.
     * @param size The size of the buffer in bytes.
     * @param data Data to upload to the buffer. Can be nullptr if no data should be uploaded.
     *
     * @return The created buffer.
     */
    Buffer* CreateBuffer(Buffer::BufferUsage bufferUsage, uint32_t size, const void* data);

    /// Create a temporary GPU buffer.
    /**
     * @param bufferUsage How the buffer will be used.
     * @param size The size of the buffer in bytes.
     * @param data Data to upload to the buffer. Can be nullptr if no data should be uploaded.
     *
     * @return The created buffer.
     */
    Buffer* CreateTemporaryBuffer(Buffer::BufferUsage bufferUsage, uint32_t size, const void* data);

  protected:
    /// The size of each buffer in the pool.
    static const uint32_t poolSize = 1024 * 1024;

  private:
    BufferAllocator(const BufferAllocator& other) = delete;

    virtual uint32_t GetAlignment(Buffer::BufferUsage bufferUsage) = 0;

    virtual RawBuffer* Allocate(Buffer::BufferUsage bufferUsage, bool temporary, unsigned int size) = 0;
    BufferAllocation AllocateTemporary(Buffer::BufferUsage bufferUsage, unsigned int size);

    virtual Buffer* CreateBufferObject(Buffer::BufferUsage bufferUsage, const BufferAllocation& allocation) = 0;
    Buffer* CreateTemporaryBufferObject(Buffer::BufferUsage bufferUsage, const BufferAllocation& allocation);

    uint32_t frame = 0;
    uint32_t frames;

    std::vector<Buffer*>* freeBufferObjects;
    std::vector<Buffer*>* usedBufferObjects;

    struct Pool {
        std::vector<RawBuffer*> rawBuffers[static_cast<uint32_t>(Buffer::BufferUsage::COUNT)];
        uint32_t currentRawBuffer[static_cast<uint32_t>(Buffer::BufferUsage::COUNT)];
        uint32_t currentOffset[static_cast<uint32_t>(Buffer::BufferUsage::COUNT)];
    };

    Pool* pools;
};

}
