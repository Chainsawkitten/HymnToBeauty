#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <Engine/Util/Json.hpp>

using namespace Component;

Camera::Camera() {}

Json::Value Camera::Save() const {
    Json::Value component;
    component["orthographic"] = orthographic;
    component["fieldOfView"] = fieldOfView;
    component["size"] = size;
    component["zNear"] = zNear;
    component["zFar"] = zFar;
    component["viewport"] = Json::SaveVec4(viewport);
    component["order"] = order;
    component["overlay"] = overlay;
    component["layerMask"] = layerMask;

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
    const float aspectRatio = screenSize.x / screenSize.y;

    if (orthographic) {
        return glm::ortho(-size * 0.5f * aspectRatio, size * 0.5f * aspectRatio, -size * 0.5f, size * 0.5f, zNear, zFar);
    } else {
        return glm::perspective(glm::radians(fieldOfView), aspectRatio, zNear, zFar);
    }
}
