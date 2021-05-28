#pragma once

#include <Video/Culling/AxisAlignedBoundingBox.hpp>
#include <vector>
#include <glm/glm.hpp>

namespace Video {
class LowLevelRenderer;
class Buffer;
class VertexDescription;
class GeometryBinding;

namespace Geometry {
/// Renderable 3D geometry.
class Geometry3D {
  public:
    /// Create new 3D geometry.
    /**
     * @param lowLevelRenderer The low-level renderer to use.
     */
    explicit Geometry3D(LowLevelRenderer* lowLevelRenderer);

    /// Destructor.
    virtual ~Geometry3D();

    /// Get the geometry binding.
    /**
     * @return The geometry binding.
     */
    GeometryBinding* GetGeometryBinding();

    /// Get number of indices.
    /**
     * @return Index count.
     */
    unsigned int GetIndexCount() const;

    /// Get the axis-aligned bounding box around the geometry.
    /**
     * @return Local space axis-aligned bounding box around the geometry.
     */
    const AxisAlignedBoundingBox& GetAxisAlignedBoundingBox() const;

    /// Get vertex position vector.
    /**
     * @return Vertex positions.
     */
    const std::vector<glm::vec3>& GetVertexPositionData() const;

    /// Get vertex index vector.
    /**
     * @return Indices.
     */
    const std::vector<uint32_t>& GetVertexIndexData() const;

  protected:
    /// Generate index buffer.
    /**
     * @param indexData Pointer to array of indices.
     * @param indexCount Number of indices.
     */
    void GenerateIndexBuffer(uint32_t* indexData, unsigned int indexCount);

    /// Generate geometry binding.
    void GenerateGeometryBinding();

    /// Create local space axis-aligned bounding box around the geometry.
    /**
     * @param positions Vector of vertex positions.
     */
    void CreateAxisAlignedBoundingBox(const std::vector<glm::vec3*>& positions);

    /// Create local space axis-aligned bounding box around the geometry.
    /**
     * @param dim Vector of vertex positions.
     * @param origin Vector of vertex positions.
     * @param minValues Vector of vertex positions.
     * @param maxValues Vector of vertex positions.
     */
    void CreateAxisAlignedBoundingBox(glm::vec3 dim, glm::vec3 origin, glm::vec3 minValues, glm::vec3 maxValues);

    /// Vertex buffer.
    Buffer* vertexBuffer = nullptr;

    /// Index buffer.
    Buffer* indexBuffer = nullptr;

    /// Vertex description.
    VertexDescription* vertexDescription = nullptr;

    /// Vertex position data.
    std::vector<glm::vec3> vertexPositionData;

    /// Vertex index data.
    std::vector<uint32_t> vertexIndexData;

    /// The low-level renderer used to create buffers and the vertex description.
    LowLevelRenderer* lowLevelRenderer;

  private:
    Video::AxisAlignedBoundingBox axisAlignedBoundingBox;
    unsigned int indexCount = 0;

    GeometryBinding* geometryBinding;
};
} // namespace Geometry
} // namespace Video
