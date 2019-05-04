#pragma once

#include "../Interface/VertexDescription.hpp"

#include <glad/glad.h>
#include <vector>

namespace Video {

/// OpenGL implementation of VertexDescription.
class OpenGLVertexDescription : public VertexDescription {
  public:
    /// Describes an OpenGL attribute in a vertex array object.
    struct OpenGLAttribute {
        /// The location of the attribute in the vertex shader.
        GLuint location;

        /// The type of each component in the attribute.
        GLenum type;

        /// The number of components in the attribute.
        GLint size;

        /// Whether the values should be normalized when accessed.
        GLboolean normalized;

        /// The offset to the first component in the buffer.
        const void* offset;
    };

    /// Create new OpenGL vertex description.
    /**
     * @param attributeCount The number of vertex attributes.
     * @param attributes The array of attributes.
     * @param indexBuffer Whether to use an index buffer.
     */
    OpenGLVertexDescription(unsigned int attributeCount, const VertexDescription::Attribute* attributes, bool indexBuffer);

    /// Destructor.
    ~OpenGLVertexDescription() final;

    /// Get the stride between vertices.
    /**
     * @return The stride between vertices in bytes.
     */
    unsigned int GetStride() const;

    /// Get the attributes.
    /**
     * @return Descriptions of the attributes in the vertex buffer(s).
     */
    const std::vector<OpenGLAttribute>& GetAttributes() const;

  private:
      OpenGLVertexDescription(const OpenGLVertexDescription& other) = delete;

      unsigned int stride;
      std::vector<OpenGLAttribute> attributes;
};

}
