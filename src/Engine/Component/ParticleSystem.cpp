#include "ParticleSystem.hpp"
#include "../Util/Json.hpp"
#include "../Manager/Managers.hpp"
#include "../Manager/ParticleManager.hpp"

Component::ParticleSystemComponent::ParticleSystemComponent() {}

Component::ParticleSystemComponent::~ParticleSystemComponent() {
    Managers().particleManager->RemoveParticleRenderer(this);
}

Json::Value Component::ParticleSystemComponent::Save() const {
    Json::Value component;

    component["textureIndex"] = particleType.textureIndex;
    component["emitAmount"] = particleType.nr_new_particles;
    component["rate"] = particleType.rate;
    component["lifetime"] = particleType.lifetime;
    component["scale"] = particleType.scale;
    component["velocity"] = Json::SaveVec3(glm::vec3(particleType.velocity));
    component["alphaControl"] = particleType.alpha_control;
    component["mass"] = particleType.mass;
    component["spread"] = particleType.spread;
    component["randomVelocity"] = Json::SaveVec3(glm::vec3(particleType.randomVec));
    component["speed"] = particleType.velocityMultiplier;
    component["NrOfParticles"] = particleType.nr_particles;

    return component;
}
