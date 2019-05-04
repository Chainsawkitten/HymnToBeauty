#pragma once

#include <Video/Geometry/Geometry2D.hpp>

namespace Video {
namespace Geometry {
/// A renderable 2D rectangle.
class Rectangle : public Video::Geometry::Geometry2D {
  public:
    /// Create new rectangle.
    /**
     * @param lowLevelRenderer The low-level renderer to use
     */
    Rectangle(LowLevelRenderer* lowLevelRenderer);

    /// Destructor.
    ~Rectangle() final;

    const Vertex* GetVertices() const final;
    unsigned int GetVertexCount() const final;
    const unsigned int* GetIndices() const final;
    unsigned int GetIndexCount() const final;

  private:
    Vertex* vertexData = nullptr;
    unsigned int vertexNr = 0;

    unsigned int* indexData = nullptr;
    unsigned int indexNr = 0;
};
} // namespace Geometry
} // namespace Video
