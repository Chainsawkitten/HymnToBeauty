#pragma once

#include <glm/glm.hpp>

namespace Video {
class Frustum;

/// A sphere.
/**
 * Used for intersection testing.
 */
class Sphere {
  public:
    /// Create new sphere.
    /**
     * @param origin Origin.
     * @param radius Radius.
     */
    Sphere(const glm::vec3& origin, float radius);

    /// Check interesection between the sphere and a frustum.
    /**
     * @param frustum The frustum to check intersection against.
     * @return Whether the bounding volumes intersect.
     */
    bool Intersects(const Frustum& frustum) const;

    /// Origin.
    glm::vec3 origin;

    /// Radius.
    float radius;
};
} // namespace Video
