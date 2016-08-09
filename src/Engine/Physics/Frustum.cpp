#include "Frustum.hpp"

#include <glm/gtc/matrix_access.hpp>

using namespace Physics;

Frustum::Frustum(const glm::mat4& matrix) {
    // Left clipping plane
    planes[0].x = glm::row(matrix, 3).x + glm::row(matrix, 0).x;
    planes[0].y = glm::row(matrix, 3).y + glm::row(matrix, 0).y;
    planes[0].z = glm::row(matrix, 3).z + glm::row(matrix, 0).z;
    planes[0].w = glm::row(matrix, 3).w + glm::row(matrix, 0).w;
    
    // Right clipping plane
    planes[1].x = glm::row(matrix, 3).x - glm::row(matrix, 0).x;
    planes[1].y = glm::row(matrix, 3).y - glm::row(matrix, 0).y;
    planes[1].z = glm::row(matrix, 3).z - glm::row(matrix, 0).z;
    planes[1].w = glm::row(matrix, 3).w - glm::row(matrix, 0).w;
    
    // Top clipping plane
    planes[2].x = glm::row(matrix, 3).x - glm::row(matrix, 1).x;
    planes[2].y = glm::row(matrix, 3).y - glm::row(matrix, 1).y;
    planes[2].z = glm::row(matrix, 3).z - glm::row(matrix, 1).z;
    planes[2].w = glm::row(matrix, 3).w - glm::row(matrix, 1).w;
    
    // Bottom clipping plane
    planes[3].x = glm::row(matrix, 3).x + glm::row(matrix, 1).x;
    planes[3].y = glm::row(matrix, 3).y + glm::row(matrix, 1).y;
    planes[3].z = glm::row(matrix, 3).z + glm::row(matrix, 1).z;
    planes[3].w = glm::row(matrix, 3).w + glm::row(matrix, 1).w;
    
    // Near clipping plane
    planes[4].x = glm::row(matrix, 3).x + glm::row(matrix, 2).x;
    planes[4].y = glm::row(matrix, 3).y + glm::row(matrix, 2).y;
    planes[4].z = glm::row(matrix, 3).z + glm::row(matrix, 2).z;
    planes[4].w = glm::row(matrix, 3).w + glm::row(matrix, 2).w;
    
    // Far clipping plane
    planes[5].x = glm::row(matrix, 3).x - glm::row(matrix, 2).x;
    planes[5].y = glm::row(matrix, 3).y - glm::row(matrix, 2).y;
    planes[5].z = glm::row(matrix, 3).z - glm::row(matrix, 2).z;
    planes[5].w = glm::row(matrix, 3).w - glm::row(matrix, 2).w;
}

float Frustum::DistanceToPoint(const glm::vec4& plane, const glm::vec3& point) {
    return plane.x * point.x + plane.y * point.y + plane.z * point.z + plane.w;
}
