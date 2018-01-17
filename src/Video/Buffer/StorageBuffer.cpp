#include "StorageBuffer.hpp"

#include <assert.h>
#include <cstring>

#include "../VideoErrorCheck.hpp"
#include <Utility/Log.hpp>

using namespace Video;

StorageBuffer::StorageBuffer(unsigned int size, GLenum usage) : bound(false) {
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, (GLsizeiptr)size, NULL, usage);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    this->size = size;
}

StorageBuffer::~StorageBuffer() {
    glDeleteBuffers(1, &ssbo);
}

void StorageBuffer::Write(void* data, unsigned int offset, unsigned int length) {
    assert(bound);
    assert(this->size >= offset + length);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, length, data); 
}

void StorageBuffer::Bind() {
    if (bound) {
        Log() << "StorageBuffer::Bind Warning: Already bound.\n";
        return;
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    bound = true;
}

void StorageBuffer::Unbind() {
    if (!bound) {
        Log() << "StorageBuffer::Unbind Warning: Not bound.\n";
        return;
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    bound = false;
}

void StorageBuffer::BindBase(unsigned int binding) const {
    { VIDEO_ERROR_CHECK("StorageBuffer::BindBase");
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, (GLuint)binding, ssbo);
    }
}

unsigned int StorageBuffer::GetSize() const {
    return size;
}
