#include "PointLight.hpp"

#include "../Util/Json.hpp"

using namespace Component;

PointLight::PointLight() {
}

Json::Value PointLight::Save() const {
    Json::Value component;
    component["color"] = Json::SaveVec3(color);
    component["attenuation"] = attenuation;
    component["intensity"] = intensity;
    component["distance"] = distance;
    return component;
}
