#pragma once

#include <Video/Geometry/Geometry3D.hpp>
#include <Video/Geometry/VertexType/StaticVertex.hpp>

namespace Geometry {
/// A cube.
class Cube : public Video::Geometry::Geometry3D {
  public:
    /// Create new cube.
    Cube();

    /// Destructor.
    ~Cube() final;

    Type GetType() const override;

  protected:
    /// Generate vertex buffer.
    void GenerateVertexBuffer(GLuint& vertexBuffer);

    /// Generate vertex array.
    void GenerateVertexArray(const GLuint vertexBuffer, const GLuint indexBuffer, GLuint& vertexArray);

  private:
    std::vector<Video::Geometry::VertexType::StaticVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<glm::vec3*> verticesPos;
};
} // namespace Geometry
