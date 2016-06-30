#include "Transform.hpp"

#include <glm/gtc/matrix_transform.hpp>

using namespace Component;

Transform::Transform(Entity* entity) : SuperComponent(entity) {
    position = glm::vec3(0.f, 0.f, 0.f);
    scale = glm::vec3(1.f, 1.f, 1.f);
    rotation = glm::vec3(0.f, 0.f, 0.f);
}

Transform::~Transform() {
    
}

glm::mat4 Transform::GetModelMatrix() const {
    glm::mat4 orientation;
    orientation = glm::rotate(orientation, glm::radians(rotation.x), glm::vec3(0.f, 1.f, 0.f));
    orientation = glm::rotate(orientation, glm::radians(rotation.y), glm::vec3(1.f, 0.f, 0.f));
    orientation = glm::rotate(orientation, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
    return glm::translate(glm::mat4(), position) * orientation * glm::scale(glm::mat4(), scale);
}

glm::mat4 Transform::GetCameraOrientation() const {
    glm::mat4 orientation;
    orientation = glm::rotate(orientation, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
    orientation = glm::rotate(orientation, glm::radians(rotation.y), glm::vec3(1.f, 0.f, 0.f));
    return glm::rotate(orientation, glm::radians(rotation.x), glm::vec3(0.f, 1.f, 0.f));
}
