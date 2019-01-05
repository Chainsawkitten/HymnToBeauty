#pragma once

#include <glad/glad.h>

namespace Video {
/// Storage buffer used to store and retrieve data on the GPU.
class StorageBuffer {
  public:
    /// Create new storage buffer.
    /**
     * @param size Size of buffer in bytes.
     * @param usage How the buffers data will be accessed.
     */
    StorageBuffer(unsigned int size, GLenum usage);

    /// Destructor.
    ~StorageBuffer();

    /// Write data to buffer.
    /**
     * @param data Pointer to data.
     * @param offset Offset in bytes from the start of the buffer to writing data.
     * @param length Length in bytes of the data to write.
     */
    void Write(void* data, unsigned int offset, unsigned int length);

    /// Bind this buffer.
    void Bind();

    /// Unbind this buffer.
    void Unbind();

    /// Bind this buffer to a binding the the shader.
    /**
     * @param binding Binding to set.
     */
    void BindBase(unsigned int binding) const;

    /// Get size of this buffer.
    /**
     * @return Size of the buffer in bytes.
     */
    unsigned int GetSize() const;

  private:
    StorageBuffer(const StorageBuffer& other) = delete;
    GLuint ssbo;
    int size;
    bool bound;
};
} // namespace Video
