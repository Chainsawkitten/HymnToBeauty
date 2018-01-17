#include "MeshData.hpp"
#include <Video/Geometry/VertexType/StaticVertex.hpp>
#include <Video/Geometry/VertexType/SkinVertex.hpp>

using namespace Geometry;

Geometry::MeshData::~MeshData() {
    if (staticVertices)
        delete[] staticVertices;

    if (skinnedVertices)
        delete[] skinnedVertices;

    if (indices)
        delete[] indices;
}
