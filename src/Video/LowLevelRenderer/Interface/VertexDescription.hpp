#pragma once

namespace Video {

class Buffer;

/// Describes how a vertex buffer is accessed by a shader.
class VertexDescription {
  public:
    /// The type of a vertex attribute.
    enum class AttributeType {
        UNSIGNED_BYTE, /// Unsigned byte.
        INT, ///< Signed integer.
        FLOAT ///< Single precision float.
    };

    /// Describes an attribute used in a vertex shader.
    struct Attribute {
        /// The number of components in the attribute.
        unsigned int size;

        /// The type of each component in the attribute.
        AttributeType type;

        /// Whether the values should be normalized when accessed.
        bool normalized;
    };

    /// Create a new vertex description.
    VertexDescription() {}

    /// Destructor.
    virtual ~VertexDescription() {}

  private:
    VertexDescription(const VertexDescription& other) = delete;
};

}
