#include "OpenGLVertexDescription.hpp"

#include "OpenGLBuffer.hpp"
#include <Utility/Log.hpp>

#include <assert.h>

namespace Video {

OpenGLVertexDescription::OpenGLVertexDescription(unsigned int attributeCount, const VertexDescription::Attribute* attributes, bool indexBuffer) {
    assert(attributeCount > 0);
    assert(attributes != nullptr);

    stride = 0;

    this->attributes.reserve(attributeCount);
    for (unsigned int i = 0; i < attributeCount; ++i) {
        assert(attributes[i].size >= 1 && attributes[i].size <= 4);

        OpenGLAttribute attribute;

        uint32_t size = 0;
        switch (attributes[i].type) {
        case AttributeType::UNSIGNED_BYTE:
            attribute.type = GL_UNSIGNED_BYTE;
            size = sizeof(uint8_t) * attributes[i].size;
            break;
        case AttributeType::INT:
            attribute.type = GL_INT;
            size = sizeof(int32_t) * attributes[i].size;
            break;
        case AttributeType::FLOAT:
            attribute.type = GL_FLOAT;
            size = sizeof(float) * attributes[i].size;
            break;
        default:
            assert(false);
        }

        attribute.location = static_cast<GLuint>(i);
        attribute.size = static_cast<GLint>(attributes[i].size);
        attribute.normalized = (attributes[i].normalized ? GL_TRUE : GL_FALSE);
        attribute.offset = stride;

        this->attributes.push_back(attribute);

        stride += size;
    }
}

OpenGLVertexDescription::~OpenGLVertexDescription() {

}

unsigned int OpenGLVertexDescription::GetStride() const {
    return stride;
}

const std::vector<OpenGLVertexDescription::OpenGLAttribute>& OpenGLVertexDescription::GetAttributes() const {
    return attributes;
}

}
