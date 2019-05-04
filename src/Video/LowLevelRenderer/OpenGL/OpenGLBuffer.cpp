#include "OpenGLBuffer.hpp"

#include <assert.h>

namespace Video {

OpenGLBuffer::OpenGLBuffer(Buffer::BufferUsage bufferUsage, unsigned int size, const void* data) : Buffer(bufferUsage) {
    assert(size > 0);

    this->size = size;

    // Map high-level buffer usage to OpenGL buffer type and usage.
    GLenum usage;
    switch (bufferUsage) {
    case BufferUsage::VERTEX_BUFFER_STATIC:
        target = GL_ARRAY_BUFFER;
        usage = GL_STATIC_DRAW;
        break;
    case BufferUsage::VERTEX_BUFFER_DYNAMIC:
        target = GL_ARRAY_BUFFER;
        usage = GL_DYNAMIC_DRAW;
        break;
    case BufferUsage::INDEX_BUFFER_STATIC:
        target = GL_ELEMENT_ARRAY_BUFFER;
        usage = GL_STATIC_DRAW;
        break;
    case BufferUsage::INDEX_BUFFER_DYNAMIC:
        target = GL_ELEMENT_ARRAY_BUFFER;
        usage = GL_DYNAMIC_DRAW;
        break;
    case BufferUsage::UNIFORM_BUFFER:
        target = GL_UNIFORM_BUFFER;
        usage = GL_DYNAMIC_DRAW;
        break;
    case BufferUsage::STORAGE_BUFFER:
        target = GL_SHADER_STORAGE_BUFFER;
        usage = GL_DYNAMIC_DRAW;
        break;
    }

    // Create buffer.
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, static_cast<GLsizeiptr>(size), (data != nullptr) ? data : NULL, usage);

    glBindBuffer(target, 0);
}

OpenGLBuffer::~OpenGLBuffer() {
    glDeleteBuffers(1, &buffer);
}

void OpenGLBuffer::Write(const void* data) {
    assert(data != nullptr);
    assert(GetBufferUsage() == BufferUsage::VERTEX_BUFFER_DYNAMIC || GetBufferUsage() == BufferUsage::INDEX_BUFFER_DYNAMIC || GetBufferUsage() == BufferUsage::UNIFORM_BUFFER || GetBufferUsage() == BufferUsage::STORAGE_BUFFER);

    glBindBuffer(target, buffer);
    glBufferSubData(target, 0, size, data);
    glBindBuffer(target, 0);
}

unsigned int OpenGLBuffer::GetSize() const {
    return size;
}

unsigned int OpenGLBuffer::GetBufferID() const {
    return buffer;
}

}
