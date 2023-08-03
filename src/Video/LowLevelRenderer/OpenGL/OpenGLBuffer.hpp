#pragma once

#include "../Interface/Buffer.hpp"
#include "../Interface/BufferAllocator.hpp"

#include <glad/gl.h>

namespace Video {

/// OpenGL implementation of Buffer.
class OpenGLBuffer : public Buffer {
  public:
    /// Create new OpenGL buffer.
    /**
     * @param bufferUsage How the buffer will be used.
     * @param allocation The allocation to encapsulate.
     */
    OpenGLBuffer(Buffer::BufferUsage bufferUsage, const BufferAllocation& allocation);

    /// Destructor.
    ~OpenGLBuffer() final;

    void Reset(BufferUsage bufferUsage, const BufferAllocation& allocation) final;
    unsigned int GetSize() const final;

    /// Get the buffer OpenGL ID.
    /**
     * @return ID to use with OpenGL commands.
     */
    GLuint GetBufferID() const;

    /// Get the OpenGL target.
    /**
     * @return The target to which the buffer is bound.
     */
    GLenum GetTarget() const;

    /// Get the offset into the raw buffer.
    /**
     * @return The offset into the raw buffer.
     */
    uint32_t GetOffset() const;

  private:
    OpenGLBuffer(const OpenGLBuffer& other) = delete;

    GLenum target;
    GLuint buffer;
    uint32_t offset;
    uint32_t size;

    RawBuffer* rawBuffer = nullptr;
    bool temporaryAllocation = false;

#ifndef NDEBUG
    bool mapped = false;
#endif
};

}
