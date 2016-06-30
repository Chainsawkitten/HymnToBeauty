#include "Transform.hpp"

using namespace Component;

Transform::Transform(Entity* entity) : SuperComponent(entity) {
    position = glm::vec3(0.f, 0.f, 0.f);
    scale = glm::vec3(1.f, 1.f, 1.f);
    rotation = glm::vec3(0.f, 0.f, 0.f);
}

Transform::~Transform() {
    
}
