#include "OpenGLGeometryBinding.hpp"

#include <assert.h>
#include "OpenGLBuffer.hpp"
#include "OpenGLVertexDescription.hpp"

namespace Video {

OpenGLGeometryBinding::OpenGLGeometryBinding(const VertexDescription* vertexDescription, const Buffer* vertexBuffer, IndexType indexType, const Buffer* indexBuffer) {
    assert(vertexDescription != nullptr);
    assert(vertexBuffer != nullptr);
    assert(indexType == IndexType::NONE || indexBuffer != nullptr);

    switch (indexType) {
    case IndexType::SHORT:
        this->indexType = GL_UNSIGNED_SHORT;
        break;
    case IndexType::INT:
        this->indexType = GL_UNSIGNED_INT;
        break;
    default:
        break;
    }

    // Create vertex array.
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    // Bind index buffer.
    if (indexBuffer != nullptr) {
        assert(indexBuffer->GetBufferUsage() == Buffer::BufferUsage::INDEX_BUFFER);
        const OpenGLBuffer* openGLIndexBuffer = static_cast<const OpenGLBuffer*>(indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, openGLIndexBuffer->GetBufferID());
        indexBufferOffset = openGLIndexBuffer->GetOffset();
    }

    // Bind vertex buffer.
    const OpenGLBuffer* openGLVertexBuffer = static_cast<const OpenGLBuffer*>(vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, openGLVertexBuffer->GetBufferID());

    // Describe vertex attributes.
    const OpenGLVertexDescription* openGLVertexDescription = static_cast<const OpenGLVertexDescription*>(vertexDescription);
    const unsigned int stride = openGLVertexDescription->GetStride();
    const std::vector<OpenGLVertexDescription::OpenGLAttribute>& attributes = openGLVertexDescription->GetAttributes();

    for (const OpenGLVertexDescription::OpenGLAttribute attribute : attributes) {
        glEnableVertexAttribArray(attribute.location);
        uintptr_t offset = attribute.offset + openGLVertexBuffer->GetOffset();
        glVertexAttribPointer(attribute.location, attribute.size, attribute.type, attribute.normalized, stride, reinterpret_cast<const void*>(offset));
    }

    glBindVertexArray(0);
}

OpenGLGeometryBinding::~OpenGLGeometryBinding() {

}

GLuint OpenGLGeometryBinding::GetVertexArray() const {
    return vertexArray;
}

GLenum OpenGLGeometryBinding::GetIndexType() const {
    return indexType;
}

uintptr_t OpenGLGeometryBinding::GetIndexBufferOffset() const {
    return indexBufferOffset;
}

}
