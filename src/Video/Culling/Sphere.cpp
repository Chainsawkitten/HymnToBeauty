#include "Sphere.hpp"

#include "Frustum.hpp"

using namespace Video;

Sphere::Sphere(const glm::vec3& origin, float radius) {
    this->origin = origin;
    this->radius = radius;
}

bool Sphere::Intersects(const Frustum& frustum) const {
    return frustum.Intersects(*this);
}
