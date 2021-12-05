#pragma once

#include "../Interface/BufferAllocator.hpp"

#include <glad/glad.h>

namespace Video {

/// OpenGL implementation of RawBuffer.
class OpenGLRawBuffer : public RawBuffer {
  public:
    /// Create new raw buffer used to satisfy allocations.
    /**
     * @param bufferUsage How the buffer will be used.
     * @param temporary Whether the buffer is going to be used to satisfy temporary allocations.
     * @param size The size of the buffer in bytes.
     */
    OpenGLRawBuffer(Buffer::BufferUsage bufferUsage, bool temporary, unsigned int size);

    /// Destructor.
    ~OpenGLRawBuffer() final;

    void Write(uint32_t offset, uint32_t size, const void* data) final;

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

  private:
    GLenum target;
    GLuint buffer;
    uint32_t size;
};

/// OpenGL implementation of BufferAllocator.
class OpenGLBufferAllocator : public BufferAllocator {
  public:
    /// Create a new buffer allocator.
    /**
     * @param frames How many frames before re-using buffers.
     */
    OpenGLBufferAllocator(uint32_t frames);

    /// Destructor.
    ~OpenGLBufferAllocator() final;

  private:
    OpenGLBufferAllocator(const OpenGLBufferAllocator& other) = delete;

    uint32_t GetAlignment(Buffer::BufferUsage bufferUsage) final;
    RawBuffer* Allocate(Buffer::BufferUsage bufferUsage, bool temporary, unsigned int size) final;
    Buffer* CreateBufferObject(Buffer::BufferUsage bufferUsage, const BufferAllocation& allocation) final;

    uint32_t bufferAlignment;
};

}
