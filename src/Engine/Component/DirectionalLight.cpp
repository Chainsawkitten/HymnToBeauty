#include "DirectionalLight.hpp"

#include "../Util/Json.hpp"

using namespace Component;

DirectionalLight::DirectionalLight() {}

void DirectionalLight::Serialize(Json::Value& node, bool load) {
    Json::Serialize(node, load, "color", color, glm::vec3(1.0f, 1.0f, 1.0f));
    Json::Serialize(node, load, "ambientCoefficient", ambientCoefficient, 0.0f);
}
