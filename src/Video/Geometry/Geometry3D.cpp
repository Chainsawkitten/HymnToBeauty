#include "Geometry3D.hpp"

using namespace Video;
using namespace Geometry;

Geometry3D::Geometry3D() {

}

Geometry3D::~Geometry3D() {
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
}

GLuint Geometry3D::GetVertexArray() const {
    return vertexArray;
}

unsigned int Geometry3D::GetIndexCount() const {
    return indexCount;
}

void Geometry3D::GenerateIndexBuffer(unsigned int* indexData, unsigned int indexCount, GLuint& indexBuffer) {
    this->indexCount = indexCount;
    glBindVertexArray(0);
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indexData, GL_STATIC_DRAW);
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
