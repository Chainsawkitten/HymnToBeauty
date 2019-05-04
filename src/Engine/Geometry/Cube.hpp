#pragma once

#include <Video/Geometry/Geometry3D.hpp>
#include <Video/Geometry/VertexType/StaticVertex.hpp>

namespace Geometry {
/// A cube.
class Cube : public Video::Geometry::Geometry3D {
  public:
    /// Create new cube.
    /**
     * @param lowLevelRenderer The low-level renderer to use
     */
    explicit Cube(Video::LowLevelRenderer* lowLevelRenderer);

    /// Destructor.
    ~Cube() final;

    Type GetType() const override;

  private:
    std::vector<Video::Geometry::VertexType::StaticVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<glm::vec3*> verticesPos;
};
} // namespace Geometry
