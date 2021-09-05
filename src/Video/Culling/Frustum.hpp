#pragma once

#include <glm/glm.hpp>

namespace Video {
class AxisAlignedBoundingBox;
class Sphere;

/// A viewing frustum.
/**
 * Used for frustum culling.
 */
class Frustum {
  public:
    /// Create new frustum.
    /**
     * @param matrix View-projection matrix to create frustum planes from.
     */
    explicit Frustum(const glm::mat4& matrix);

    /// Check intersection between frustum and an axis-aligned bounding box.
    /**
     * @param aabb The axis-aligned bounding box to check intersection against.
     * @return Whether the bounding volumes intersect.
     */
    bool Intersects(const AxisAlignedBoundingBox& aabb) const;

    /// Check intersection between frustum and a sphere.
    /**
     * @param sphere The sphere to check intersection against.
     * @return Whether the bounding volumes intersect.
     */
    bool Intersects(const Sphere& sphere) const;

  private:
    glm::vec4 planes[6];

    // The distance returned is the signed distance in normal vector units.
    // To ensure the "true" distance the plane equation has to be normalized.
    static float DistanceToPoint(const glm::vec4& plane, const glm::vec3& point);
};
} // namespace Video
