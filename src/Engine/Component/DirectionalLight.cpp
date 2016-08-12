#include "DirectionalLight.hpp"

#include "../Util/Json.hpp"

using namespace Component;

DirectionalLight::DirectionalLight(Entity* entity) : SuperComponent(entity) {
    
}

Json::Value DirectionalLight::Save() const {
    Json::Value component;
    component["color"] = Json::SaveVec3(color);
    component["ambientCoefficient"] = ambientCoefficient;
    return component;
}

void DirectionalLight::Load(const Json::Value& node) {
    color = Json::LoadVec3(node["color"]);
    ambientCoefficient = node.get("ambientCoefficient", 0.5f).asFloat();
}
