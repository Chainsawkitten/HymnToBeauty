#include "AxisAlignedBoundingBox.hpp"

namespace Physics {
    AxisAlignedBoundingBox::AxisAlignedBoundingBox(const glm::vec3& dimensions, const glm::vec3& origin, const glm::vec3& minVertex, const glm::vec3& maxVertex) {
        this->dimensions = dimensions;
        this->origin = origin;
        this->minVertex = minVertex;
        this->maxVertex = maxVertex;
    }
}
