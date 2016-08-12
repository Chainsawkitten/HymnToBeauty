#include "SpotLight.hpp"

#include "../Util/Json.hpp"

using namespace Component;

SpotLight::SpotLight(Entity* entity) : SuperComponent(entity) {
    
}

Json::Value SpotLight::Save() const {
    Json::Value component;
    component["color"] = Json::SaveVec3(color);
    component["ambientCoefficient"] = ambientCoefficient;
    component["attenuation"] = attenuation;
    component["intensity"] = intensity;
    component["coneAngle"] = coneAngle;
    return component;
}

void SpotLight::Load(const Json::Value& node) {
    color = Json::LoadVec3(node["color"]);
    ambientCoefficient = node.get("ambientCoefficient", 0.5f).asFloat();
    attenuation = node.get("attenuation", 1.f).asFloat();
    intensity = node.get("intensity", 1.f).asFloat();
    coneAngle = node.get("coneAngle", 15.f).asFloat();
}
