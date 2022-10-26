#pragma once

#include <glm/glm.hpp>

class Entity;
namespace Utility {
class Window;
}

namespace MousePicking {

/// Get the direction of the ray in world space. Normalized.
/**
 * @param camera The camera the mouse picking makes calculations from.
 * @param projection The camera's projection matrix.
 * @param window The window to pick in.
 */
glm::vec3 GetRayDirection(const Entity* camera, const glm::mat4& projection, const Utility::Window* window);

}
