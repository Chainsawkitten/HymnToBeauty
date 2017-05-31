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
    component["averageLifetime"] = particleType.averageLifetime;
    component["lifetimeVariance"] = particleType.lifetimeVariance;
    component["averageSize"] = Json::SaveVec2(particleType.averageSize);
    component["sizeVariance"] = Json::SaveVec2(particleType.sizeVariance);
    component["uniformScaling"] = particleType.uniformScaling;
    component["startAlpha"] = particleType.startAlpha;
    component["midAlpha"] = particleType.midAlpha;
    component["endAlpha"] = particleType.endAlpha;
    component["color"] = Json::SaveVec3(particleType.color);
    component["size"] = Json::SaveVec3(size);
    component["averageEmitTime"] = averageEmitTime;
    component["emitTimeVariance"] = emitTimeVariance;
    component["emitterType"] = emitterType;
    
    return component;
}

void ParticleEmitter::Load(const Json::Value& node) {
    particleType.textureIndex = node.get("textureIndex", 0).asInt();
    particleType.minVelocity = Json::LoadVec3(node["minVelocity"]);
    particleType.maxVelocity = Json::LoadVec3(node["maxVelocity"]);
    particleType.averageLifetime = node.get("averageLifetime", 0.f).asFloat();
    particleType.lifetimeVariance = node.get("lifetimeVariance", 0.f).asFloat();
    particleType.averageSize = Json::LoadVec2(node["averageSize"]);
    particleType.sizeVariance = Json::LoadVec2(node["sizeVariance"]);
    particleType.uniformScaling = node.get("uniformScaling", false).asBool();
    particleType.startAlpha = node.get("startAlpha", 0.f).asFloat();
    particleType.midAlpha = node.get("midAlpha", 1.f).asFloat();
    particleType.endAlpha = node.get("endAlpha", 0.f).asFloat();
    particleType.color = Json::LoadVec3(node["color"]);
    size = Json::LoadVec3(node["size"]);
    averageEmitTime = node.get("averageEmitTime", 0.03).asFloat();
    emitTimeVariance = node.get("emitTimeVariance", 0.03).asFloat();
    emitterType = static_cast<EmitterType>(node.get("emitterType", 0).asInt());
}
