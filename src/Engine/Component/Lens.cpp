#include "Lens.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

using namespace Component;

Lens::Lens(Entity* entity) : SuperComponent(entity) {
    fieldOfView = 45.f;
    zFar = 1000.f;
    zNear = 0.05f;
}

Lens::~Lens() {
}

glm::mat4 Lens::GetProjection(const glm::vec2& screenSize) const {
    return glm::perspective(glm::radians(fieldOfView), screenSize.x/screenSize.y, zNear, zFar);
}
