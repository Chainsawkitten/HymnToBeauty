#pragma once

#include <glm/glm.hpp>

namespace Video {
class LowLevelRenderer;
class Buffer;
class VertexDescription;

namespace Geometry {
namespace VertexType {
/// Vertex type used for static meshes.
struct StaticVertex {
    /// Position.
    glm::vec3 position;

    /// %Texture coordinate.
    glm::vec2 textureCoordinate;

    /// Normal.
    glm::vec3 normal;

    /// Tangent vector.
    glm::vec3 tangent;

    /// Generate vertex buffer
    /**
     * @param lowLevelRenderer The low-level renderer to use.
     * @param vertices Pointer to vertex array.
     * @param vertexCount Number of vertices.
     *
     * @return Vertex buffer.
     */
    static Buffer* GenerateVertexBuffer(LowLevelRenderer* lowLevelRenderer, StaticVertex* vertices, unsigned int vertexCount);

    /// Generate vertex description.
    /**
     * @param lowLevelRenderer The low-level renderer to use.
     *
     * @return Vertex description.
     */
    static VertexDescription* GenerateVertexDescription(LowLevelRenderer* lowLevelRenderer);
};
} // namespace VertexType
} // namespace Geometry
} // namespace Video
