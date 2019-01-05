#include "SpotLight.hpp"

#include "../Util/Json.hpp"

using namespace Component;

SpotLight::SpotLight() {}

Json::Value SpotLight::Save() const {
    Json::Value component;
    component["color"] = Json::SaveVec3(color);
    component["ambientCoefficient"] = ambientCoefficient;
    component["attenuation"] = attenuation;
    component["intensity"] = intensity;
    component["coneAngle"] = coneAngle;
    component["shadow"] = shadow;
    component["distance"] = distance;
    return component;
}
