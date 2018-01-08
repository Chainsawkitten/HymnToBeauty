#include "Lens.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

using namespace Component;

Lens::Lens() {
    
}

Json::Value Lens::Save() const {
    Json::Value component;
    component["fieldOfView"] = fieldOfView;
    component["zNear"] = zNear;
    component["zFar"] = zFar;
    return component;
}

glm::mat4 Lens::GetProjection(const glm::vec2& screenSize) const {
    return glm::perspective(glm::radians(fieldOfView), screenSize.x/screenSize.y, zNear, zFar);
}
