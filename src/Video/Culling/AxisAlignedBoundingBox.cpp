#include "AxisAlignedBoundingBox.hpp"

#include "Frustum.hpp"

using namespace Video;

AxisAlignedBoundingBox::AxisAlignedBoundingBox(const glm::vec3& dimensions, const glm::vec3& origin, const glm::vec3& minVertex, const glm::vec3& maxVertex) {
    this->dimensions = dimensions;
    this->origin = origin;
    this->minVertex = minVertex;
    this->maxVertex = maxVertex;
}

bool AxisAlignedBoundingBox::Collide(const Frustum& frustum) const {
    return frustum.Collide(*this);
}
