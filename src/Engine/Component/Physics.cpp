#include "Physics.hpp"

#include "../Util/Json.hpp"

using namespace Component;

Physics::Physics(Entity* entity) : SuperComponent(entity) {
    
}

Json::Value Physics::Save() const {
    Json::Value component;
    component["velocity"] = Json::SaveVec3(velocity);
    component["maxVelocity"] = maxVelocity;
    component["angularVelocity"] = Json::SaveVec3(angularVelocity);
    component["maxAngularVelocity"] = maxAngularVelocity;
    component["acceleration"] = Json::SaveVec3(acceleration);
    component["angularAcceleration"] = Json::SaveVec3(angularAcceleration);
    component["velocityDragFactor"] = velocityDragFactor;
    component["angularDragFactor"] = angularDragFactor;
    component["gravityFactor"] = gravityFactor;
    component["momentOfInertia"] = Json::SaveVec3(momentOfInertia);
    return component;
}

void Physics::Load(const Json::Value& node) {
    velocity = Json::LoadVec3(node["velocity"]);
    maxVelocity = node.get("maxVelocity", 20.f).asFloat();
    angularVelocity = Json::LoadVec3(node["angularVelocity"]);
    maxAngularVelocity = node.get("maxAngularVelocity", 2.f).asFloat();
    acceleration = Json::LoadVec3(node["acceleration"]);
    angularAcceleration = Json::LoadVec3(node["angularAcceleration"]);
    velocityDragFactor = node.get("velocityDragFactor", 1.f).asFloat();
    angularDragFactor = node.get("angularDragFactor", 1.f).asFloat();
    gravityFactor = node.get("gravityFactor", 0.f).asFloat();
    momentOfInertia = Json::LoadVec3(node["momentOfInertia"]);
}
