#pragma once

#include "../Interface/GeometryBinding.hpp"

#include <glad/glad.h>

namespace Video {

class VertexDescription;
class Buffer;

/// OpenGL implementation of GeometryBinding.
class OpenGLGeometryBinding : public GeometryBinding {
  public:
    /// Create new OpenGL geometry binding.
    /**
     * @param vertexDescription Description of the attributes in the vertex buffer.
     * @param vertexBuffer Buffer containing the vertex data.
     * @param indexType The type of values in the index buffer.
     * @param indexBuffer Index buffer.
     */
    OpenGLGeometryBinding(const VertexDescription* vertexDescription, const Buffer* vertexBuffer, IndexType indexType = IndexType::NONE, const Buffer* indexBuffer = nullptr);

    /// Destructor.
    ~OpenGLGeometryBinding() final;

    /// Get the vertex array object.
    /**
     * @return The vertex array to use in OpenGL calls.
     */
    GLuint GetVertexArray() const;

    /// Get the type of the values in the index buffer.
    /**
     * @return The type of the values in the index buffer.
     */
    GLenum GetIndexType() const;

  private:
    OpenGLGeometryBinding(const OpenGLGeometryBinding& other) = delete;

    GLuint vertexArray;
    GLenum indexType;
};

}
