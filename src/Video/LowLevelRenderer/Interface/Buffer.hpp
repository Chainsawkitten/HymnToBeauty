#pragma once

namespace Video {

/// A buffer containing GPU accessible data.
class Buffer {
  public:
    /// How the buffer is going to be used.
    enum class BufferUsage {
        VERTEX_BUFFER_STATIC, ///< The buffer will be used as a vertex buffer and only written to once.
        VERTEX_BUFFER_DYNAMIC, ///< The buffer with be used as a vertex buffer and written to many times.
        INDEX_BUFFER_STATIC, ///< The buffer will be used as an index buffer and only written to once.
        INDEX_BUFFER_DYNAMIC, ///< The buffer will be used as an index buffer and written to many times.
        UNIFORM_BUFFER, ///< The buffer will be used as a uniform buffer.
        STORAGE_BUFFER ///< The buffer will be used as a storage buffer.
    };

    /// Access specified when mapping a buffer.
    enum class Access {
        READ_ONLY, ///< The mapped memory will be read, but not written.
        WRITE_ONLY, ///< The mapped memory will be written, but not read.
        READ_WRITE ///< The mapped memory will be both read and written.
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

    /// Write data to buffer.
    /**
     * The buffer's data can only be written once per frame.
     * @param data Data to write to the buffer.
     */
    virtual void Write(const void* data) = 0;

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

  private:
    Buffer(const Buffer& other) = delete;

    BufferUsage bufferUsage;
};

}
