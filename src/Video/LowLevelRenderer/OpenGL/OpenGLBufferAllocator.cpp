#include "OpenGLBufferAllocator.hpp"

#include "OpenGLBuffer.hpp"
#include <cassert>

namespace Video {

OpenGLRawBuffer::OpenGLRawBuffer(Buffer::BufferUsage bufferUsage, bool temporary, unsigned int size) {
    assert(size > 0);

    this->size = size;

    // Map high-level buffer usage to OpenGL buffer type and usage.
    GLenum usage;
    switch (bufferUsage) {
    case Buffer::BufferUsage::VERTEX_BUFFER:
        target = GL_ARRAY_BUFFER;
        break;
    case Buffer::BufferUsage::INDEX_BUFFER:
        target = GL_ELEMENT_ARRAY_BUFFER;
        break;
    case Buffer::BufferUsage::UNIFORM_BUFFER:
        target = GL_UNIFORM_BUFFER;
        break;
    case Buffer::BufferUsage::STORAGE_BUFFER:
    case Buffer::BufferUsage::VERTEX_STORAGE_BUFFER:
        target = GL_SHADER_STORAGE_BUFFER;
        break;
    }

    usage = temporary ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

    // Create buffer.
    glGenBuffers(1, &buffer);

    glBindBuffer(target, buffer);
    glBufferData(target, static_cast<GLsizeiptr>(size), NULL, usage);

    glBindBuffer(target, 0);
}

OpenGLRawBuffer::~OpenGLRawBuffer() {
    glDeleteBuffers(1, &buffer);
}

void OpenGLRawBuffer::Write(uint32_t offset, uint32_t size, const void* data) {
    assert(size > 0);
    assert(offset + size <= this->size);
    assert(data != nullptr);

    glBindBuffer(target, buffer);
    glBufferSubData(target, offset, size, data);
    glBindBuffer(target, 0);
}

GLuint OpenGLRawBuffer::GetBufferID() const {
    return buffer;
}

GLenum OpenGLRawBuffer::GetTarget() const {
    return target;
}

OpenGLBufferAllocator::OpenGLBufferAllocator(uint32_t frames) : BufferAllocator(frames) {
    GLint alignment;
    glGetIntegerv(GL_MIN_MAP_BUFFER_ALIGNMENT, &alignment);
    bufferAlignment = static_cast<uint32_t>(alignment);
}

OpenGLBufferAllocator::~OpenGLBufferAllocator() {

}

uint32_t OpenGLBufferAllocator::GetAlignment(Buffer::BufferUsage bufferUsage) {
    return bufferAlignment;
}

RawBuffer* OpenGLBufferAllocator::Allocate(Buffer::BufferUsage bufferUsage, bool temporary, unsigned int size) {
    return new OpenGLRawBuffer(bufferUsage, temporary, size);
}

Buffer* OpenGLBufferAllocator::CreateBufferObject(Buffer::BufferUsage bufferUsage, const BufferAllocation& allocation) {
    return new OpenGLBuffer(bufferUsage, allocation);
}

}
