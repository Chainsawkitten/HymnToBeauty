#include "Geometry3D.hpp"

#include "../LowLevelRenderer/Interface/LowLevelRenderer.hpp"
#include "../LowLevelRenderer/Interface/Buffer.hpp"

using namespace Video;
using namespace Geometry;

Geometry3D::Geometry3D(LowLevelRenderer* lowLevelRenderer) {
    this->lowLevelRenderer = lowLevelRenderer;
}

Geometry3D::~Geometry3D() {
    delete vertexBuffer;
    delete indexBuffer;
    delete geometryBinding;
    delete vertexDescription;
}

GeometryBinding* Geometry3D::GetGeometryBinding() {
    return geometryBinding;
}

unsigned int Geometry3D::GetIndexCount() const {
    return indexCount;
}

const Video::AxisAlignedBoundingBox& Geometry3D::GetAxisAlignedBoundingBox() const {
    return axisAlignedBoundingBox;
}

const std::vector<glm::vec3>& Geometry3D::GetVertexPositionData() const {
    return vertexPositionData;
}

const std::vector<uint32_t>& Geometry3D::GetVertexIndexData() const {
    return vertexIndexData;
}

void Geometry3D::GenerateIndexBuffer(uint32_t* indexData, unsigned int indexCount) {
    this->indexCount = indexCount;
    indexBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::INDEX_BUFFER_STATIC, indexCount * sizeof(uint32_t), indexData);
}

void Geometry3D::GenerateGeometryBinding() {
    geometryBinding = lowLevelRenderer->CreateGeometryBinding(vertexDescription, vertexBuffer, Video::GeometryBinding::IndexType::INT, indexBuffer);
}

void Geometry3D::CreateAxisAlignedBoundingBox(const std::vector<glm::vec3*>& positions) {
    glm::vec3 minValues, maxValues, origin, dim;
    minValues = maxValues = origin = glm::vec3(0.f, 0.f, 0.f);

    // Find minimum/maximum bounding points.
    for (std::size_t i = 0; i < positions.size(); ++i) {
        const glm::vec3& pos = *positions[i];
        if (pos.x > maxValues.x)
            maxValues.x = pos.x;
        else if (pos.x < minValues.x)
            minValues.x = pos.x;

        if (pos.y > maxValues.y)
            maxValues.y = pos.y;
        else if (pos.y < minValues.y)
            minValues.y = pos.y;

        if (pos.z > maxValues.z)
            maxValues.z = pos.z;
        else if (pos.z < minValues.z)
            minValues.z = pos.z;
    }

    // Set origin.
    origin.x = (minValues.x + maxValues.x) / 2.f;
    origin.y = (minValues.y + maxValues.y) / 2.f;
    origin.z = (minValues.z + maxValues.z) / 2.f;

    // Dimensions.
    dim.x = maxValues.x - minValues.x;
    dim.y = maxValues.y - minValues.y;
    dim.z = maxValues.z - minValues.z;
    axisAlignedBoundingBox = Video::AxisAlignedBoundingBox(dim, origin, minValues, maxValues);
}

void Geometry3D::CreateAxisAlignedBoundingBox(glm::vec3 dim, glm::vec3 origin, glm::vec3 minValues, glm::vec3 maxValues) {
    axisAlignedBoundingBox = Video::AxisAlignedBoundingBox(dim, origin, minValues, maxValues);
}
