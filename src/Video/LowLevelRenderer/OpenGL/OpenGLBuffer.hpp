#pragma once

#include "../Interface/Buffer.hpp"

#include <glad/glad.h>

namespace Video {

/// OpenGL implementation of Buffer.
class OpenGLBuffer : public Buffer {
  public:
    /// Create new OpenGL buffer.
    /**
     * @param bufferUsage How the buffer will be used.
     * @param size The size of the buffer in bytes.
     * @param data Data to upload to the buffer. Can be nullptr if no data should be uploaded.
     */
    OpenGLBuffer(Buffer::BufferUsage bufferUsage, unsigned int size, const void* data = nullptr);

    /// Destructor.
    ~OpenGLBuffer() final;

    void Write(const void* data) final;
    unsigned int GetSize() const final;

    /// Get the buffer OpenGL ID.
    /**
     * @return ID to use with OpenGL commands.
     */
    unsigned int GetBufferID() const;

  private:
    OpenGLBuffer(const OpenGLBuffer& other) = delete;

    GLenum target;
    GLuint buffer;
    unsigned int size;

#ifndef NDEBUG
    bool mapped = false;
#endif
};

}
