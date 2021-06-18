#pragma once

#include <cstdint>

namespace Video {

class LowLevelRenderer;
class Buffer;
class VertexDescription;
class GeometryBinding;

/// An isocahedron used as proxy light geometry.
class Isocahedron {
  public:
    /// Create new isocahedron.
    /**
     * @param lowLevelRenderer The low-level renderer.
     */
    explicit Isocahedron(LowLevelRenderer* lowLevelRenderer);

    /// Destructor.
    ~Isocahedron();

    /// Get the isocahedron's vertex description.
    /**
     * @return The vertex description.
     */
    const VertexDescription* GetVertexDescription() const;

    /// Get the geometry binding.
    /**
     * @return The geometry binding.
     */
    GeometryBinding* GetGeometryBinding();

    /// Get the number of indices in the geometry.
    /**
     * @return The number of indices in the geometry.
     */
    uint16_t GetIndexCount() const;

    /// Get the error when compared to a sphere.
    /**
     * @return The error.
     */
    float GetError() const;

  private:
    VertexDescription* vertexDescription;
    Buffer* vertexBuffer;
    Buffer* indexBuffer;
    GeometryBinding* geometryBinding;
    float error;
};

}
