#pragma once

namespace Video {

struct BufferAllocation;

/// A buffer containing GPU accessible data.
class Buffer {
  public:
    /// How the buffer is going to be used.
    enum class BufferUsage {
        VERTEX_BUFFER, ///< The buffer will be used as a vertex buffer.
        INDEX_BUFFER, ///< The buffer will be used as an index buffer.
        UNIFORM_BUFFER, ///< The buffer will be used as a uniform buffer.
        STORAGE_BUFFER, ///< The buffer will be used as a storage buffer.
        VERTEX_STORAGE_BUFFER, ///< The buffer will be used as both vertex and storage buffer.
        COUNT ///< The number of different buffer usages.
    };
    
    /// Create a new buffer.
    /**
     * @param bufferUsage How the buffer will be used.
     */
    Buffer(BufferUsage bufferUsage) {
        this->bufferUsage = bufferUsage;
    }

    /// Destructor.
    virtual ~Buffer() {}

    /// Reset the buffer.
    /** 
     * @param bufferUsage How the buffer will be used.
     * @param allocation Allocation to back up buffer memory.
     */
    virtual void Reset(BufferUsage bufferUsage, const BufferAllocation& allocation) = 0;

    /// Get the size of the buffer.
    /**
     * @return The size of the buffer in bytes.
     */
    virtual unsigned int GetSize() const = 0;

    /// Get how the buffer is to be used.
    /**
     * @return How the buffer will be used.
     */
    BufferUsage GetBufferUsage() const {
        return bufferUsage;
    }

  protected:
    /// How the buffer will be used.
    BufferUsage bufferUsage;

  private:
    Buffer(const Buffer& other) = delete;
};

}
