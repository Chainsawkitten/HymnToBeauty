#pragma once

#include <glm/glm.hpp>

namespace Video {
class LowLevelRenderer;
class Buffer;
class VertexDescription;

namespace Geometry {
namespace VertexType {
/// Vertex type used for rigged (animated) meshes.
struct SkinVertex {
    /// Position.
    glm::vec3 position;

    /// %Texture coordinate.
    glm::vec2 textureCoordinate;

    /// Normal.
    glm::vec3 normal;

    /// Tangent vector.
    glm::vec3 tangent;

    /// Bone IDs.
    glm::ivec4 boneIDs;

    /// Bone weights.
    glm::vec4 weights;

    /// Generate vertex buffer.
    /**
     * @param lowLevelRenderer The low-level renderer to use.
     * @param vertices Pointer to vertex array.
     * @param vertexCount Number of vertices.
     *
     * @return Vertex buffer.
     */
    static Buffer* GenerateVertexBuffer(LowLevelRenderer* lowLevelRenderer, SkinVertex* vertices, unsigned int vertexCount);

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
