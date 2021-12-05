#include "OpenGLBuffer.hpp"

#include <Utility/Log.hpp>
#include "OpenGLBufferAllocator.hpp"
#include <assert.h>

namespace Video {

OpenGLBuffer::OpenGLBuffer(Buffer::BufferUsage bufferUsage, const BufferAllocation& allocation) : Buffer(bufferUsage) {
    Reset(bufferUsage, allocation);
}

OpenGLBuffer::~OpenGLBuffer() {
    if (rawBuffer != nullptr) {
        if (!temporaryAllocation)
            delete rawBuffer;
    } else {
        glDeleteBuffers(1, &buffer);
    }
}

void OpenGLBuffer::Reset(BufferUsage bufferUsage, const BufferAllocation& allocation) {
    this->bufferUsage = bufferUsage;
    rawBuffer = allocation.buffer;
    OpenGLRawBuffer* openGLRawBuffer = static_cast<OpenGLRawBuffer*>(rawBuffer);
    buffer = openGLRawBuffer->GetBufferID();
    target = openGLRawBuffer->GetTarget();
    offset = allocation.offset;
    size = allocation.size;
    temporaryAllocation = allocation.temporaryAllocation;
}

uint32_t OpenGLBuffer::GetSize() const {
    return size;
}

GLuint OpenGLBuffer::GetBufferID() const {
    return buffer;
}

GLenum OpenGLBuffer::GetTarget() const {
    return target;
}

uint32_t OpenGLBuffer::GetOffset() const {
    return offset;
}

}
