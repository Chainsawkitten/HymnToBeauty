#include "Lens.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

using namespace Component;

Lens::Lens(Entity* entity) : SuperComponent(entity) {
    
}

Json::Value Lens::Save() const {
    Json::Value component;
    component["fieldOfView"] = fieldOfView;
    component["zNear"] = zNear;
    component["zFar"] = zFar;
    return component;
}

void Lens::Load(const Json::Value& node) {
    fieldOfView = node.get("fieldOfView", 45.f).asFloat();
    zNear = node.get("zNear", 0.5f).asFloat();
    zFar = node.get("zFar", 100.f).asFloat();
}

glm::mat4 Lens::GetProjection(const glm::vec2& screenSize) const {
    return glm::perspective(glm::radians(fieldOfView), screenSize.x/screenSize.y, zNear, zFar);
}
