#include "Lens.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

using namespace Component;

Lens::Lens(Entity* entity) : SuperComponent(entity) {
    
}

Lens::~Lens() {
}

glm::mat4 Lens::GetProjection(const glm::vec2& screenSize) const {
    return glm::perspective(glm::radians(fieldOfView), screenSize.x/screenSize.y, zNear, zFar);
}
