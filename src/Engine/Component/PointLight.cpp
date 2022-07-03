#include "PointLight.hpp"

#include "../Util/Json.hpp"

using namespace Component;

PointLight::PointLight() {}

void PointLight::Serialize(Json::Value& node, bool load) {
    Json::Serialize(node, load, "color", color, glm::vec3(1.0f, 1.0f, 1.0f));
    Json::Serialize(node, load, "attenuation", attenuation, 1.0f);
    Json::Serialize(node, load, "intensity", intensity, 1.0f);
    Json::Serialize(node, load, "distance", distance, 1.0f);
}
