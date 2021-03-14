#pragma once

#include <glm/glm.hpp>

namespace Geometry {

/// Calculate the dot product of two quaternions.
/**
 * @param q1 First quaternion.
 * @param q2 Second quaternion.
 * @return t Dot product value.
 */
float DotQuat(const glm::quat& q1, const glm::quat& q2);

/// Normalize quaternion.
/**
 * @param q Quaternion to normalize.
 */
void NormalizeQuat(glm::quat& q);

} // namespace Geometry
