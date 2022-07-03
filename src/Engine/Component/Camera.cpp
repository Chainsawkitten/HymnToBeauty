#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <Engine/Util/Json.hpp>

using namespace Component;

Camera::Camera() {}

void Camera::Serialize(Json::Value& node, bool load) {
    Json::Serialize(node, load, "orthographic", orthographic, false);
    Json::Serialize(node, load, "fieldOfView", fieldOfView, 45.0f);
    Json::Serialize(node, load, "size", size, 10.0f);
    Json::Serialize(node, load, "zNear", zNear, 0.1f);
    Json::Serialize(node, load, "zFar", zFar, 100.0f);
    Json::Serialize(node, load, "viewport", viewport, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
    Json::Serialize(node, load, "order", order, 0);
    Json::Serialize(node, load, "overlay", overlay, false);
    Json::Serialize(node, load, "layerMask", layerMask, 1u);
    
    Json::Value& filtersNode = node["filters"];
    Json::Serialize(filtersNode, load, "tonemapping", filterSettings.tonemapping, false);
    Json::Serialize(filtersNode, load, "gamma", filterSettings.gamma, 2.2f);
    Json::Serialize(filtersNode, load, "ditherApply", filterSettings.ditherApply, false);
    Json::Serialize(filtersNode, load, "fxaa", filterSettings.fxaa, false);
    Json::Serialize(filtersNode, load, "bloom", filterSettings.bloom, false);
    Json::Serialize(filtersNode, load, "bloomIntensity", filterSettings.bloomIntensity, 1.0f);
    Json::Serialize(filtersNode, load, "bloomThreshold", filterSettings.bloomThreshold, 1.0f);
    Json::Serialize(filtersNode, load, "bloomScatter", filterSettings.bloomScatter, 0.7f);
}

glm::mat4 Camera::GetProjection(const glm::vec2& screenSize) const {
    const float aspectRatio = screenSize.x / screenSize.y;

    if (orthographic) {
        return glm::ortho(-size * 0.5f * aspectRatio, size * 0.5f * aspectRatio, -size * 0.5f, size * 0.5f, zNear, zFar);
    } else {
        return glm::perspective(glm::radians(fieldOfView), aspectRatio, zNear, zFar);
    }
}
