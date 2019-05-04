#pragma once

#include <glm/glm.hpp>

namespace Video {
class LowLevelRenderer;
class Buffer;
class VertexDescription;
class GeometryBinding;

namespace Geometry {
/// Interface for renderable 2D geometry.
class Geometry2D {
  public:
    /// A vertex point.
    struct Vertex {
        /// Position.
        glm::vec2 position;

        /// %Texture coordinate.
        glm::vec2 textureCoordinate;
    };

    /// Create new 2D geometry.
    /**
     * @param lowLevelRenderer The low-level renderer to use.
     */
    explicit Geometry2D(LowLevelRenderer* lowLevelRenderer);

    /// Destructor
    virtual ~Geometry2D();

    /// Get all the vertices.
    /**
     * @return Array of vertices
     */
    virtual const Vertex* GetVertices() const = 0;

    /// Get the number of vertices.
    /**
     * @return The number of vertices
     */
    virtual unsigned int GetVertexCount() const = 0;

    /// Get all the vertex indices.
    /**
     * @return Array of vertex indices
     */
    virtual const unsigned int* GetIndices() const = 0;

    /// Get the number of indicies.
    /**
     * @return The number of vertex indices.
     */
    virtual unsigned int GetIndexCount() const = 0;

    /// Get the geometry binding.
    /**
     * @return The geometry binding.
     */
    const GeometryBinding* GetGeometryBinding() const;

  protected:
    /// Generate vertex and index buffers.
    void GenerateBuffers();

    /// Generate geometry binding.
    void GenerateGeometryBinding();

  private:
    LowLevelRenderer* lowLevelRenderer;
    Buffer* vertexBuffer;
    Buffer* indexBuffer;
    VertexDescription* vertexDescription;
    GeometryBinding* geometryBinding;
};
} // namespace Geometry
} // namespace Video
