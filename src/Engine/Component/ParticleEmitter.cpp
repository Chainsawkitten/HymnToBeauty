#include "ParticleEmitter.hpp"

#include "../Util/Json.hpp"

using namespace Component;

ParticleEmitter::ParticleEmitter(Entity* entity) : SuperComponent(entity) {
    
}

Json::Value ParticleEmitter::Save() const {
    Json::Value component;
    
    component["textureIndex"] = particleType.textureIndex;
    component["minVelocity"] = Json::SaveVec3(particleType.minVelocity);
    component["maxVelocity"] = Json::SaveVec3(particleType.maxVelocity);
    component["minLifetime"] = particleType.minLifetime;
    component["maxLifetime"] = particleType.maxLifetime;
    component["minSize"] = Json::SaveVec2(particleType.minSize);
    component["maxSize"] = Json::SaveVec2(particleType.maxSize);
    component["uniformScaling"] = particleType.uniformScaling;
    component["startAlpha"] = particleType.startAlpha;
    component["midAlpha"] = particleType.midAlpha;
    component["endAlpha"] = particleType.endAlpha;
    component["color"] = Json::SaveVec3(particleType.color);
    component["size"] = Json::SaveVec3(size);
    component["minEmitTime"] = minEmitTime;
    component["maxEmitTime"] = maxEmitTime;
    component["emitterType"] = emitterType;
    
    return component;
}

void ParticleEmitter::Load(const Json::Value& node) {
    particleType.textureIndex = node.get("textureIndex", 0).asInt();
    particleType.minVelocity = Json::LoadVec3(node["minVelocity"]);
    particleType.maxVelocity = Json::LoadVec3(node["maxVelocity"]);
    particleType.minLifetime = node.get("minLifetime", 0.f).asFloat();
    particleType.maxLifetime = node.get("maxLifetime", 0.f).asFloat();
    particleType.minSize = Json::LoadVec2(node["minSize"]);
    particleType.maxSize = Json::LoadVec2(node["maxSize"]);
    particleType.uniformScaling = node.get("uniformScaling", false).asBool();
    particleType.startAlpha = node.get("startAlpha", 0.f).asFloat();
    particleType.midAlpha = node.get("midAlpha", 1.f).asFloat();
    particleType.endAlpha = node.get("endAlpha", 0.f).asFloat();
    particleType.color = Json::LoadVec3(node["color"]);
    size = Json::LoadVec3(node["size"]);
    minEmitTime = node.get("minEmitTime", 0.03).asFloat();
    maxEmitTime = node.get("maxEmitTime", 0.03).asFloat();
    emitterType = static_cast<EmitterType>(node.get("emitterType", 0).asInt());
}
