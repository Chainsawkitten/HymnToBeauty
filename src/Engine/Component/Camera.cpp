#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <Engine/Util/Json.hpp>

using namespace Component;

Camera::Camera() {}

Json::Value Camera::Save() const {
    Json::Value component;
    component["fieldOfView"] = fieldOfView;
    component["zNear"] = zNear;
    component["zFar"] = zFar;
    component["viewport"] = Json::SaveVec4(viewport);

    Json::Value filtersNode;
    filtersNode["gamma"] = filterSettings.gamma;
    filtersNode["dither"] = filterSettings.ditherApply;
    filtersNode["fxaa"] = filterSettings.fxaa;
    filtersNode["bloom"] = filterSettings.bloom;
    filtersNode["bloomIntensity"] = filterSettings.bloomIntensity;
    filtersNode["bloomThreshold"] = filterSettings.bloomThreshold;
    filtersNode["bloomScatter"] = filterSettings.bloomScatter;
    component["filters"] = filtersNode;

    return component;
}

glm::mat4 Camera::GetProjection(const glm::vec2& screenSize) const {
    return glm::perspective(glm::radians(fieldOfView), screenSize.x / screenSize.y, zNear, zFar);
}
