#include "MeshData.hpp"
#include <Video/Geometry/VertexType/StaticVertex.hpp>

using namespace Geometry;

Geometry::MeshData::~MeshData() {
    if (staticVertices)
        delete[] staticVertices;

    if (indices)
        delete[] indices;
}
