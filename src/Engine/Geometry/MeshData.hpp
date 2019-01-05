#pragma once

#include <cinttypes>
#include <glm/glm.hpp>

namespace Video {
namespace Geometry {
namespace VertexType {
struct StaticVertex;
struct SkinVertex;
} // namespace VertexType
} // namespace Geometry
} // namespace Video
namespace Geometry {
/// Mesh data.
struct MeshData {
    /// Destructor.
    ~MeshData();

    /// Parent id of mesh.
    uint32_t parent;

    /// Number of vertices.
    uint32_t numVertices;

    /// Number of indicies.
    uint32_t numIndices;

    /// Axis aligned bound box dimensions.
    glm::vec3 aabbDim;

    /// Axis aligned bound box origin.
    glm::vec3 aabbOrigin;

    /// Minimum position of axis aligned bound box.
    glm::vec3 aabbMinpos;

    /// Maximum position of axis aligned bound box.
    glm::vec3 aabbMaxpos;

    /// Is skinned.
    bool isSkinned = false;

    /// Static vertices.
    Video::Geometry::VertexType::StaticVertex* staticVertices = nullptr;

    /// Skinned vertices.
    Video::Geometry::VertexType::SkinVertex* skinnedVertices = nullptr;

    /// Array of indices.
    uint32_t* indices = nullptr;

    /// Store in cpu?
    bool CPU;

    /// Store in gpu?
    bool GPU;
};
} // namespace Geometry
