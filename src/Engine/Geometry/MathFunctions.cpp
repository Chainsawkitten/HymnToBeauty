#include "MathFunctions.hpp"
#include <glm/gtc/quaternion.hpp>

float Geometry::DotQuat(const glm::quat& q1, const glm::quat& q2) {
    return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
}

void Geometry::NormalizeQuat(glm::quat& q) {
    float d = sqrt(DotQuat(q, q));
    if (d >= 0.00001) {
        d = 1 / d;
        q.x *= d;
        q.y *= d;
        q.z *= d;
        q.w *= d;
    } else {
        q.x = q.y = q.z = 0;
        q.w = 1;
    }
}
