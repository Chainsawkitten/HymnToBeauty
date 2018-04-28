#include "DirectionalLight.hpp"

#include "../Util/Json.hpp"

using namespace Component;

DirectionalLight::DirectionalLight() {
}

Json::Value DirectionalLight::Save() const {
    Json::Value component;
    component["color"] = Json::SaveVec3(color);
    component["ambientCoefficient"] = ambientCoefficient;
    return component;
}
