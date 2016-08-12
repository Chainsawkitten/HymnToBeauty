#include "PointLight.hpp"

#include "../Util/Json.hpp"

using namespace Component;

PointLight::PointLight(Entity* entity) : SuperComponent(entity) {
    
}

Json::Value PointLight::Save() const {
    Json::Value component;
    component["color"] = Json::SaveVec3(color);
    component["ambientCoefficient"] = ambientCoefficient;
    component["attenuation"] = attenuation;
    component["intensity"] = intensity;
    return component;
}

void PointLight::Load(const Json::Value& node) {
    color = Json::LoadVec3(node["color"]);
    ambientCoefficient = node.get("ambientCoefficient", 0.5f).asFloat();
    attenuation = node.get("attenuation", 1.f).asFloat();
    intensity = node.get("intensity", 1.f).asFloat();
}
